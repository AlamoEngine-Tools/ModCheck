#ifndef ASSETS_MTD_H
#define ASSETS_MTD_H

#include "Assets/Files.h"
#include <set>

class MegaTextureDirectory
{
    std::set<std::string> m_files;

public:
    bool exists(const std::string& filename) const;
    MegaTextureDirectory(const ptr<File> f);
};

#endif