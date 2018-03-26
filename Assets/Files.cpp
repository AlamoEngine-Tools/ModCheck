#include "Assets/Files.h"
#include "General/Exceptions.h"
#include <cassert>
using namespace std;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
This structure represents a single file handle, which can be shared by 
multiple File instances. It is reference counted and created when a File
is created from the filesystem.
*/
struct File::Info
{
    HANDLE       m_hFile;       // Handle of the file
    size_t       m_size;        // Size of the file
    size_t       m_cursor;      // Current cursor info the file
    unsigned int m_references;  // #References to this instance

    // Set the file pointer
    void Seek(size_t offset)
    {
        if (m_cursor != offset)
        {
            m_cursor = SetFilePointer(m_hFile, (LONG)offset, NULL, FILE_BEGIN);
        }
    }

    size_t Read(void* buffer, size_t size)
    {
        // Size should have already been sanitized
        assert(m_cursor + size <= m_size);

        DWORD read;
        if (!ReadFile(m_hFile, buffer, (DWORD)size, &read, NULL))
        {
            throw ReadException();
        }
        m_cursor += read;
        return read;
    }

    Info(HANDLE hFile)
        : m_hFile(hFile), m_cursor(0), m_references(1)
    {
	    m_size = GetFileSize(m_hFile, NULL);
    }

    ~Info()
    {
        CloseHandle(m_hFile);
    }
};

size_t File::SetPosition(size_t position)
{
    return m_cursor = min(position, m_size);
}

size_t File::Read(void* buffer, size_t size)
{
    // Sanitize input
    size = min(m_size - m_cursor, size);

    m_info->Seek(m_base + m_cursor);
    size_t read = m_info->Read(buffer, size);
    m_cursor += read;
    return read;
}

File* File::Open(const wstring& path, const string& name)
{
    HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // Couldn't open file, find out why
	    DWORD error = GetLastError();
	    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
	    {
            // File or directory does not exist; do not throw an exception (too slow)
		    return NULL;
	    }
	    throw IOException(L"Unable to open file: " + path);
    }
    
    // File opened; return the file object
    return new File(new Info(hFile), name);
}

// Creates this file by opening a file from the filesystem
File::File(Info* info, const string& name)
    : m_info(info),
      m_base(0), m_cursor(0), m_size(m_info->m_size),
      m_name(name)
{
}

// Creates this file as a subfile of an existing file.
File::File(const File& f, unsigned long offset, size_t size, const string& name)
    : m_info(f.m_info), m_name(name),
      // Sanitize offset and size to stay within the file f.
      m_base( min(offset, f.GetSize()) ),
      m_cursor(0),
      m_size( min(size, f.GetSize() - m_base) )
{
    // Add the base of f to our base to get the absolute file offset
    // where this file starts.
    m_base += f.m_base;

    // Add a reference
    m_info->m_references++;
}

File::~File()
{
    if (--m_info->m_references == 0)
    {
        delete m_info;
    }
}
