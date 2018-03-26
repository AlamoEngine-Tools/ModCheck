#include "Assets/XML.h"
#include "General/Exceptions.h"
#include <cassert>
#include <sstream>
using namespace std;

static const int BUFFER_SIZE = 32*1024;	// Read this much at once

class ExpatParseException : public ParseException
{
public:
    ExpatParseException(const string& message) : ParseException(message) {}
};

struct ParseData
{
    XMLTree*    tree;
    std::string filename;
    XML_Parser* parser;
};

XMLNode::~XMLNode()
{
	for (XMLNode* next, *node = m_firstChild; node != NULL; node = next)
	{
        next = node->m_next;
		delete node;
	}
}

static void onStartElement(void* userData, const XML_Char *name, const XML_Char **atts)
{
    ParseData* data = (ParseData*)userData;
	XMLTree*   tree = data->tree;

    // Create the node
    XMLNode* node = new XMLNode(Location(data->filename, XML_GetCurrentLineNumber(*data->parser)));
    node->m_parent     = tree->m_currentNode;
    node->m_name       = tree->append(name, strlen(name) + 1);
    node->m_data       = NULL;
    node->m_firstChild = NULL;
    node->m_lastChild  = NULL;
	while (*atts != NULL)
	{
        XML_Char* aname  = tree->append(atts[0], strlen(atts[0]) + 1);
        XML_Char* avalue = tree->append(atts[1], strlen(atts[1]) + 1);
		node->m_attributes.push_back(make_pair(aname, avalue));
		atts += 2;
	}

	if (tree->m_currentNode == NULL)
	{
        // This is the root
	    delete tree->m_root;
		tree->m_root = node;
	}
	else
	{
        // Append to the current node's children
		if (tree->m_currentNode->m_firstChild == NULL) {
            tree->m_currentNode->m_firstChild = node;
        } else {
            assert(tree->m_currentNode->m_lastChild != NULL);
            tree->m_currentNode->m_lastChild->m_next = node;
        }
        tree->m_currentNode->m_lastChild = node;
	}
	tree->m_currentNode = node;
    tree->m_currentData.resize(0);
}

static void onEndElement(void* userData, const XML_Char *name)
{
    ParseData* data = (ParseData*)userData;
	XMLTree*   tree = data->tree;

	if (tree->m_currentNode != NULL)
	{
        tree->m_currentNode->m_next = NULL;
        if (tree->m_currentNode->m_firstChild == NULL && !tree->m_currentData.empty())
        {
            tree->m_currentData.push_back('\0'); // Append the NUL terminator

            // Set the collected node data; but trim it first
            char* begin = &tree->m_currentData.front();
            char* end   = &tree->m_currentData.back();
            
            while (begin != end && isspace(*begin)) begin++;
            if (begin != end)
            {
                while (end != begin && isspace(*(end - 1))) end--;
                *end = '\0';
                tree->m_currentNode->m_data = tree->append(begin, end - begin + 1);
            }
        }
        
        // Move up the tree
		tree->m_currentNode = tree->m_currentNode->m_parent;
	}
}

static void onCharacterData(void *userData, const XML_Char *s, int len)
{
    ParseData* data = (ParseData*)userData;
	XMLTree*   tree = data->tree;

    if (tree->m_currentNode != NULL && tree->m_currentNode->m_firstChild == NULL)
	{
        // Only nodes without child nodes can have data
        tree->m_currentData.insert(tree->m_currentData.end(), s, s + len);
	}
}

// Appends a string into the m_data array and returns a pointer to the added string
XML_Char* XMLTree::append(const XML_Char* start, size_t length)
{
    // The vector should not resize
    assert(m_data.size() + length <= m_data.capacity());

    m_data.insert(m_data.end(), start, start + length);
    return &m_data.back() + 1 - length;
}

XMLTree::XMLTree(File& file)
{
	m_root        = NULL;
	m_currentNode = NULL;

	XML_Parser parser = XML_ParserCreate(NULL);
	if (parser == NULL)
	{
		throw runtime_error("Unable to create XML parser");
	}

	try
	{
        ParseData data;
        data.tree     = this;
        data.filename = file.GetName();
        data.parser   = &parser;

	    XML_SetUserData(parser, &data);
	    XML_SetElementHandler(parser, onStartElement, onEndElement);
	    XML_SetCharacterDataHandler(parser, onCharacterData);

        // The m_data buffer will contain all names and other strings in the file.
        // Since this can never exceed the file size, we reserve that much and can then
        // simply use direct pointers into the container.
        m_data.reserve(file.GetSize());

        file.SetPosition(0);
		while (!file.IsEOF())
		{
            char buffer [ BUFFER_SIZE ];
			size_t n = file.Read(buffer, BUFFER_SIZE);
			if (XML_Parse(parser, buffer, (int)n, file.IsEOF()) == 0)
			{
                stringstream ss;
                ss << file.GetName() << ":" << XML_GetCurrentLineNumber(parser)
                   << ": " << XML_ErrorString(XML_GetErrorCode(parser));
                throw ExpatParseException(ss.str());
			}
		}

        // Cleanup
        m_currentNode = NULL;
        m_currentData.clear();
        XML_ParserFree(parser);
    }
	catch (...)
	{
        delete m_root;
		XML_ParserFree(parser);
		throw;
	}

    if (m_root == NULL)
    {
        stringstream ss;
        ss << file.GetName() << ":" << 1 << ": root element missing";
        throw ExpatParseException(ss.str());
    }

    if (m_root->begin() == m_root->end())
    {
        // The root element should have elements
        stringstream ss;
        ss << m_root->filename << ":" << m_root->line << ": root element may not be empty";
        throw ExpatParseException(ss.str());
    }
}

XMLTree::~XMLTree()
{
	delete m_root;
}
