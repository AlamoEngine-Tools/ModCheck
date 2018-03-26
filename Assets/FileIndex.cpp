#include "Assets/Assets.h"
#include "Assets/FileIndex.h"
#include "General/Utils.h"
#include "General/ExactTypes.h"
#include "General/Exceptions.h"
#include <algorithm>
using namespace std;

namespace Assets
{
#ifndef NDEBUG
// Uncomment this define to show a log of all assets being loaded
//#define DEBUG_ASSETS
#endif

FileIndex                 g_FileIndex;
std::vector<std::wstring> g_SearchPaths;

static ptr<File> LoadPhysicalFile(const wstring& basepath, const string& filename)
{
    try
    {
        wstring path = basepath + Utils::ConvertAnsiStringToWideString(filename);
        ptr<File> f = File::Open(path, filename);
#ifdef DEBUG_ASSETS
        if (f != NULL)
        {
            printf("Loading %ls\n", path.c_str());
        }
#endif
        return f;
    }
    catch (FileNotFoundException&)
    {
    }
    return NULL;
}

static ptr<File> LoadVirtualFile(const std::string& filename_)
{
    const string filename = Utils::Uppercase(filename_);
    for (FileIndex::const_iterator p = g_FileIndex.begin(); p != g_FileIndex.end(); ++p)
    {
        if (filename.compare(0, p->base.length(), p->base) == 0)
        {
            map<string,FileInfo>::const_iterator q = p->files.find(filename.substr(p->base.length()));
            if (q != p->files.end())
            {
                // Create and return the subfile
#ifdef DEBUG_ASSETS
                printf("Loading %s\n", filename_.c_str());
#endif
                return new File(*p->file, q->second.start, q->second.size, filename_);
            }
        }
    }
    return NULL;
}

ptr<File> LoadFile(string filename, const char* const * Extensions)
{
    // Strip the extension of the filename
    string ext;
    string::size_type sep = filename.find_last_of("\\/");
    string::size_type dot = filename.find_last_of(".");
    if (dot != string::npos && dot > sep) {
        ext = filename.substr(dot);
        filename.erase(dot);
    }

    // Not in the cache, try to load the file
    ptr<File> file;
    try
    {
        // Search for the physical file in the specified search paths.
        for (vector<wstring>::const_iterator p = g_SearchPaths.begin(); file == NULL && p != g_SearchPaths.end(); ++p)
        {
            // First try with the given extension
            if (((file = LoadPhysicalFile(*p, filename + ext)) == NULL) && Extensions != NULL)
            {
                // Not found, try with the alternative extensions, if any
                for (size_t i = 0; Extensions[i] != NULL; i++)
                {
                    if ((file = LoadPhysicalFile(*p, filename + "." + Extensions[i])) != NULL)
                    {
                        // Found it
                        break;
                    }
                }
            }
        }

        if (file == NULL)
        {
            // Not found, try file index
            // First try with the given extension
            if (((file = LoadVirtualFile(filename + ext)) == NULL) && Extensions != NULL)
            {
                // Not found, try with the alternative extensions
                for (size_t i = 0; Extensions[i] != NULL; i++)
                {
                    if ((file = LoadVirtualFile(filename + "." + Extensions[i])) != NULL)
                    {
                        // Found it
                        break;
                    }
                }
            }
        }
    }
    catch (IOException&)
    {
    }

    return file;
}

ptr<File> LoadFile(const std::string& filename)
{
    return LoadFile(filename, NULL);
}

static void IndexMegaFile(ptr<File> f, const char* base = "")
{
    // Read file header
    struct {
        uint32_t nFilenames;
        uint32_t nFiles;
    } hdr;
    if (f->Read(&hdr, sizeof hdr) != sizeof hdr)
    {
        throw ReadException();
    }

    // Read filenames
    vector<string> filenames(letohl(hdr.nFilenames));
    vector<char>   filename;
    for (size_t i = 0; i < filenames.size(); i++)
    {
        uint16_t leLength;
        if (f->Read(&leLength, sizeof leLength) != sizeof leLength)
        {
            throw ReadException();
        }
        unsigned int length = letohs(leLength);
        filename.resize(length);
        if (f->Read(&filename[0], length) != length)
        {
            throw ReadException();
        }
        filenames[i] = string(&filename[0], length);
    }

    g_FileIndex.push_back(MegaFileInfo());
    MegaFileInfo& mfi = g_FileIndex.back();
    mfi.base = base;
    mfi.file = f;

    // Read files
    unsigned long nFiles = letohl(hdr.nFiles);
    for (unsigned long i = 0; i < nFiles; i++)
    {
        struct
        {
            uint32_t crc;
            uint32_t index;
            uint32_t size;
            uint32_t start;
            uint32_t nameIndex;
        } info;

        if (f->Read(&info, sizeof info) != sizeof info)
        {
            throw ReadException();
        }

        FileInfo& fi = mfi.files[filenames[letohl(info.nameIndex)]];
        fi.start  = letohl(info.start);
        fi.size   = letohl(info.size);
    }
}

static void ProcessMegaFileIndex(File& f, const wstring& path = wstring())
{
    XMLTree xml(f);
    const XMLNode& root = xml.GetRoot();
    for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
    {
        const char* filename = p->GetData();
        if (filename != NULL)
        {
            ptr<File> f = path.empty() ? LoadFile(filename) : LoadPhysicalFile(path, filename);
            if (f != NULL)
            {
	            IndexMegaFile(f);
            }
        }
    }
}

static wstring SanitizePath(const wstring& src)
{
    wstring path(src);
    // Replace slash with backslash
    replace(path.begin(), path.end(), '/', '\\');
    // Make sure the path ends in a directory seperator.
    if (*path.rbegin() != L'\\') path += L"\\";
    return path;
}

void Initialize(const wstring& mod_path, const wstring& main_path, const wstring& old_path)
{
#ifdef DEBUG_ASSETS
    printf("Initializing assets:\n");
#endif

    g_SearchPaths.clear();
    if (!mod_path.empty())  g_SearchPaths.push_back(SanitizePath(mod_path));
    if (!main_path.empty()) g_SearchPaths.push_back(SanitizePath(main_path));

    ptr<File> f;
    // Load the content
    if ((f = LoadFile("Data\\Patch.meg"))                           != NULL) IndexMegaFile(f);
    if ((f = LoadFile("Data\\MegaFiles.xml"))                       != NULL) ProcessMegaFileIndex(*f, SanitizePath(main_path));
    if ((f = LoadFile("Data\\Audio\\SFX\\SFX2D_English.meg"))       != NULL) IndexMegaFile(f, "DATA\\AUDIO\\SFX\\");
	if ((f = LoadFile("Data\\Audio\\SFX\\SFX2D_Non_Localized.meg")) != NULL) IndexMegaFile(f, "DATA\\AUDIO\\SFX\\");
	if ((f = LoadFile("Data\\Audio\\SFX\\SFX3D_Non_Localized.meg")) != NULL) IndexMegaFile(f, "DATA\\AUDIO\\SFX\\");

    if (!old_path.empty())
    {
        // If we have a backup path, load those contents as well
        wstring path = SanitizePath(old_path);
        if ((f = LoadPhysicalFile(path, "Data\\Patch.meg"))                           != NULL) IndexMegaFile(f);
        if ((f = LoadPhysicalFile(path, "Data\\MegaFiles.xml"))                       != NULL) ProcessMegaFileIndex(*f, path);
        if ((f = LoadPhysicalFile(path, "Data\\Audio\\SFX\\SFX2D_English.meg"))       != NULL) IndexMegaFile(f, "DATA\\AUDIO\\SFX\\");
        if ((f = LoadPhysicalFile(path, "Data\\Audio\\SFX\\SFX2D_Non_Localized.meg")) != NULL) IndexMegaFile(f, "DATA\\AUDIO\\SFX\\");
        if ((f = LoadPhysicalFile(path, "Data\\Audio\\SFX\\SFX3D_Non_Localized.meg")) != NULL) IndexMegaFile(f, "DATA\\AUDIO\\SFX\\");
        
        g_SearchPaths.push_back(path);
    }


#ifdef DEBUG_ASSETS
    size_t total = 0;
    for (FileIndex::const_iterator p = g_FileIndex.begin(); p != g_FileIndex.end(); ++p)
    {
        total += p->files.size();
    }
    printf("\n");
    printf("\nAsset initialization done (%u files in index)\n\n", total);
#endif
}

void Uninitialize()
{
    g_FileIndex.clear();
    g_SearchPaths.clear();
}
}