#ifndef ASSETS_FILEINDEX_H
#define ASSETS_FILEINDEX_H

#include "Assets/Files.h"
#include "General/Objects.h"
#include <map>
#include <list>
#include <vector>

namespace Assets
{
    struct FileInfo
    {
        unsigned long start;
        unsigned long size;
    };

    struct MegaFileInfo
    {
        ptr<File>                       file;
        std::string                     base;
        std::map<std::string, FileInfo> files;
    };

    typedef std::map<std::string, ptr<File> > AssetCache;
    typedef std::list<MegaFileInfo>           FileIndex;

    extern FileIndex                 g_FileIndex;
    extern std::vector<std::wstring> g_SearchPaths;

    ptr<File> LoadFile(std::string filename, const char* const * Extensions);
}

#endif