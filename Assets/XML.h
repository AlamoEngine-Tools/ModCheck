/*
Defines a XMLTree class that holds an XML document parsed into a tree.
It uses a slight restriction for XML; nodes can either contain child
nodes or data, but not both.
*/
#ifndef ASSETS_XML_H
#define ASSETS_XML_H

#include "Assets/Files.h"
#include "expat/expat.h"
#include <vector>

class XMLTree;

struct Location
{
    std::string filename;
    int         line;

    bool operator != (const Location& rhs) const {
        return (line != rhs.line || filename != rhs.filename);
    }

    bool operator < (const Location& rhs) const {
        return (filename < rhs.filename) || (!(rhs.filename < filename) && line < rhs.line);
    }

    Location(const std::string& filename, int line = -1)
        : filename(filename), line(line) {}
};

/*
Represents a node in an XML Tree.
Note that pointers to XMLNode's, as handed out by XMLTree and XMLNode are valid only
during the lifetime of the XMLTree variable.
*/
class XMLNode : public Location
{
	friend static void onStartElement (void* userData, const XML_Char *name, const XML_Char **atts);
	friend static void onEndElement   (void* userData, const XML_Char *name);
	friend static void onCharacterData(void *userData, const XML_Char *s, int len);
	friend class XMLTree;

    // Attribute list is a vector of <name, value> pairs
    typedef std::vector< std::pair<XML_Char*,XML_Char*> > AttrList;

	XMLNode*  m_parent;
	XML_Char* m_name;
	XML_Char* m_data;
	AttrList  m_attributes;
	XMLNode*  m_firstChild;
    union {
        XMLNode*  m_next;
        XMLNode*  m_lastChild;
    };

    XMLNode(const Location& loc) : Location(loc) {}
	~XMLNode();

public:
    class const_iterator
    {
        const XMLNode* m_cur;
    public:
    	const XMLNode* operator->() const { return m_cur; }
        operator const XMLNode*()   const { return m_cur; }
        const_iterator& operator++() { m_cur = m_cur->m_next; return *this; }
        const_iterator(const XMLNode* cur) : m_cur(cur) {}
    };

    const_iterator begin() const { return const_iterator(m_firstChild); }
    const_iterator end()   const { return const_iterator(NULL); }

	bool           IsAnonymous() const      { return strcmp(m_name,"") == 0; }
	const char*    GetData() const          { return m_data; }
	const char*    GetName() const          { return m_name; }
	bool           Equals(const char* name)        const { return _stricmp(m_name, name) == 0; }
	bool           Equals(const std::string& name) const { return Equals(name.c_str()); }

	
    /* Returns the value of the specified attribute.
     * Returns NULL if the attribute does not exist.
     * @name: case-insensitive name of the attribute.
     */
	const char* GetAttribute(const char* name) const {
        for (AttrList::const_iterator i = m_attributes.begin(); i != m_attributes.end(); i++) {
            if (_stricmp(i->first, name) == 0) {
                return i->second;
            }
        }
		return NULL;
	}

    const char* GetAttribute(const std::string& name) const {
        return GetAttribute(name.c_str());
    }
};

// Represents a XML tree built from a XML file
class XMLTree
{
	friend static void onStartElement(void* userData, const XML_Char *name, const XML_Char **atts);
	friend static void onEndElement(void* userData, const XML_Char *name);
	friend static void onCharacterData(void *userData, const XML_Char *s, int len);

    std::vector<XML_Char> m_data;
	XMLNode*              m_root;

    // Used during parsing
	XMLNode*              m_currentNode;
    std::vector<XML_Char> m_currentData;

    XML_Char* append(const XML_Char* start, size_t length);

public:
    // Returns the root node
	const XMLNode& GetRoot() const { return *m_root; }

    // Builds the tree using the specified XML file
	XMLTree(File& file);
	~XMLTree();
};

#endif