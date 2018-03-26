#include "Assets/StringList.h"
#include "General/Utils.h"
#include "General/ExactTypes.h"
#include "General/Exceptions.h"
using namespace std;

#pragma pack(1)
struct Entry
{
    uint32_t crc;
    uint32_t nValChars;
    uint32_t nKeyChars;
};
#pragma pack()

struct StringInfo
{
    unsigned long nValChars;
    unsigned long nKeyChars;
    unsigned long nValOffset;
    unsigned long nKeyOffset;
};

bool StringList::exists(const std::string& name) const
{
    return m_strings.find(name) != m_strings.end();
}

StringList::StringList(ptr<File> f)
{
    uint32_t leCount;
    if (f->Read(&leCount, sizeof leCount) != sizeof leCount)
    {
        throw ReadException();
    }

    unsigned long nValOffset = 0;
    unsigned long nKeyOffset = 0;

    vector<StringInfo> strings(letohl(leCount));
    for (size_t i = 0; i < strings.size(); i++)
    {
        Entry entry;
        if (f->Read(&entry, sizeof entry) != sizeof entry)
        {
            throw ReadException();
        }
        strings[i].nKeyOffset = nKeyOffset;
        strings[i].nValOffset = nValOffset;
        nKeyOffset += (strings[i].nKeyChars = letohl(entry.nKeyChars));
        nValOffset += (strings[i].nValChars = letohl(entry.nValChars));
    }

    vector<wchar_t> values(nValOffset);
    vector<char>    keys(nKeyOffset);
    if (f->Read(&values[0], values.size() * sizeof(wchar_t)) != values.size() * sizeof(wchar_t) ||
        f->Read(&keys[0], keys.size() * sizeof(char)) != keys.size() * sizeof(char))
    {
        throw ReadException();
    }

    for (size_t i = 0; i < strings.size(); i++)
    {
        m_strings.insert(make_pair(
             string(&keys  [strings[i].nKeyOffset], strings[i].nKeyChars),
            wstring(&values[strings[i].nValOffset], strings[i].nValChars)));
    }
}
