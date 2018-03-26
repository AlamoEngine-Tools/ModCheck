#ifndef ASSETS_FILES_H
#define ASSETS_FILES_H

#include "General/Objects.h"
#include <string>

class File : public Object
{
    struct Info;

    Info*        m_info;
    size_t       m_base;
    size_t       m_cursor;
    size_t       m_size;
    std::string  m_name;
    std::wstring m_path;

    File(const File& f) {} // No copying allowed
    File(Info* info, const std::string& name);
public:
    size_t Read(void* buffer, size_t size);
    size_t SetPosition(size_t position);
    
    size_t GetPosition() const { return m_cursor; }
    size_t GetSize()     const { return m_size;   }
    bool   IsEOF()       const { return m_cursor >= m_size; }

    const std::string&  GetName() const { return m_name; }

    static File* Open(const std::wstring& path, const std::string& name);

    File(const File& f, unsigned long base, size_t size, const std::string& name);
    ~File();
};

#endif