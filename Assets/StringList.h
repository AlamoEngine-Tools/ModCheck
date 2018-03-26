#ifndef ASSETS_STRINGLIST_H
#define ASSETS_STRINGLIST_H

#include "Assets/Files.h"
#include <map>

class StringList
{
    std::map<std::string, std::wstring> m_strings;

public:
    bool exists(const std::string& name) const;

    StringList(ptr<File> f);
};

#endif