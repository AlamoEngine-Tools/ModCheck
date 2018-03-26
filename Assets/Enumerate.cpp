#include "Assets/Assets.h"
#include "Assets/FileIndex.h"
#include "General/Exceptions.h"
#include "General/Utils.h"
#include <algorithm>
#include <cassert>
#include <set>
#include <stack>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
using namespace std;

namespace Assets {

class Enumerator : public IEnumerator
{
    struct FileInfo
    {
        std::string name;
        size_t      size;
        
        bool operator < (const FileInfo& rhs) const {
            return _stricmp(name.c_str(), rhs.name.c_str()) < 0;
        }
    };

    set<FileInfo>                 m_files;
    set<FileInfo>::const_iterator m_cursor;

    // Checks if str[ofs,...] matches the filter[ofs2,...]
    static bool MatchFilter(const string& str, string::size_type ofs, const string& filter, string::size_type ofs2)
    {
        for (; ofs != str.length() && ofs2 != filter.length(); ++ofs, ++ofs2)
        {
            const char f = filter[ofs2];
            if (f == '*')
            {
                // Greedy match operator. Try to match the rest of the string with
                // the rest of the filter. Start from the back.
                for (string::size_type p = str.length(); p != ofs; --p)
                {
                    if (MatchFilter(str, p, filter, ofs2 + 1))
                    {
                        return true;
                    }
                }
                return false;
            }

            // We don't care at ?, just go to the next.
            // Otherwise, it's a literal and must match.
            if (f != '?' && str[ofs] != f)
            {
                return false;
            }
        }
        // Match only if both reached the end
        return (ofs == str.length() && ofs2 == filter.length());
    }
    
    void EnumerateDirectory(const wstring& search_path, const wstring& basedir, const wstring& wfilter)
    {
        WIN32_FIND_DATA wfd;
        wstring         filter_path;
        HANDLE          hFind;
        
        // First find all normal files in the directory that match
        filter_path = search_path + basedir + wfilter;
        hFind = FindFirstFile(filter_path.c_str(), &wfd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (~wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    // Regular file that matches the mask
                    FileInfo info;
                    info.name = Utils::ConvertWideStringToAnsiString(basedir + wfd.cFileName);
                    info.size = wfd.nFileSizeLow;
                    m_files.insert(info);
                }
            } while (FindNextFile(hFind, &wfd));
            FindClose(hFind);
        }

        // Then find all directories and recurse
        filter_path = search_path + basedir + L"*";
        hFind = FindFirstFile(filter_path.c_str(), &wfd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
                    wcscmp(wfd.cFileName,L".") != 0 && wcscmp(wfd.cFileName,L"..") != 0)
                {
                    EnumerateDirectory(search_path, basedir + wfd.cFileName + L"\\", wfilter);
                }
            } while (FindNextFile(hFind, &wfd));
            FindClose(hFind);
        }
    }

    void FindPhysicalFiles(const string& filter)
    {
        wstring wfilter(filter.length(), L' ');
        copy(filter.begin(), filter.end(), wfilter.begin());
        const wstring basedir = Utils::GetBasePath(wfilter);
        wfilter = Utils::GetFilename(wfilter);

        // Find all the files in the physical search paths
        for (size_t i = 0; i < g_SearchPaths.size(); i++)
        {
            EnumerateDirectory(g_SearchPaths[i], basedir, wfilter);
        }
    }

    void FindVirtualFiles(string filter)
    {
        // Get the base directory part
        string base = Utils::GetBasePath(filter);
        filter = Utils::GetFilename(filter);
        transform(filter.begin(), filter.end(), filter.begin(), toupper);

        // Check all MegaFiles
        for (FileIndex::const_iterator q = g_FileIndex.begin(); q != g_FileIndex.end(); q++)
        {
            string megbase = Utils::Uppercase(base);    // Base for this megafile
            string prefix;

            if (megbase.length() >= q->base.length())
            {
                // Base in filter is longer (or equal) than base of MegaFile
                if (megbase.compare(0, q->base.length(), q->base) != 0) {
                    // Bases don't match; this MegaFile cannot
                    // contain any files we want.
                    continue;
                }
                // Remove matching part
                megbase.erase(0, q->base.length());
            }
            else
            {
                // Base in MegaFile is longer than base in filter
                prefix = q->base.substr(megbase.length());
                megbase.clear();
            }

            // Iterate over the files while the base matches
            for (map<string, Assets::FileInfo>::const_iterator p = q->files.lower_bound(megbase);
                p != q->files.end() && p->first.compare(0, megbase.length(), megbase) == 0;
                ++p)
            {
                string::size_type start = p->first.find_last_of("\\/");
                start = max(megbase.length(), (start != string::npos) ? start + 1 : 0);

                if (MatchFilter(prefix + p->first, start, filter, 0))
                {
                    FileInfo info;
                    info.name = q->base + p->first;
                    info.size = p->second.size;
                    m_files.insert(info);
                }
            }
        }
    }

public:
    const string& GetFileName() const { return m_cursor->name; }
    size_t        GetFileSize() const { return m_cursor->size; }

    bool Next()
    {
        if (m_cursor != m_files.end())
        {
            ++m_cursor;
            return m_cursor != m_files.end();
        }
        return false;
    }

    Enumerator(string filter)
    {
        // Normalize filter
        replace(filter.begin(), filter.end(), '/', '\\');

        FindPhysicalFiles(filter);
        FindVirtualFiles(filter);

        if (m_files.empty())
        {
            // We didn't find any files
            throw FileNotFoundException(L"");
        }
        m_cursor = m_files.begin();
    }
};

ptr<IEnumerator> Enumerate(const std::string& filter)
{
    try
    {
        return new Enumerator(filter);
    }
    catch (FileNotFoundException&)
    {
    }
    return NULL;
}

}
