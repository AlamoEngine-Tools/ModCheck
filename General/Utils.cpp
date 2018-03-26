#include "General/Utils.h"
#include <algorithm>
#include <cassert>
#include <functional>
using namespace std;

namespace Utils {

// Calculates the 32-bit Cyclic Redundancy Checksum (CRC-32) of a block of data
unsigned long CRC32(const void *data, size_t size)
{
    static unsigned long lookupTable[256];
    static bool          validTable = false;

	if (!validTable)
	{
        // Initialize table
	    for (int i = 0; i < 256; i++)
        {
		    unsigned long crc = i;
            for (int j = 0; j < 8; j++)
		    {
			    crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320 : (crc >> 1);
		    }
            lookupTable[i] = crc & 0xFFFFFFFF;
	    }
	    validTable = true;
	}

	unsigned long crc = 0xFFFFFFFF;
	for (size_t j = 0; j < size; j++)
	{
		crc = ((crc >> 8) & 0x00FFFFFF) ^ lookupTable[ (crc ^ ((const char*)data)[j]) & 0xFF ];
	}
	return crc ^ 0xFFFFFFFF;
}

// Converts an ANSI string to a Unicode string
wstring ConvertAnsiStringToWideString(const char* str)
{
    wstring wide(strlen(str), L'\0');
    copy(str, str + wide.length(), wide.begin());
    return wide;
}

wstring ConvertAnsiStringToWideString(const string& str)
{
    wstring wide(str.length(), L'\0');
    copy(str.begin(), str.end(), wide.begin());
    return wide;
}

// Converts a Wide (Unicide) string to an ANSI string
string ConvertWideStringToAnsiString(const wchar_t* str)
{
    string ansi(wcslen(str), L'\0');
    for (string::iterator d = ansi.begin(); d != ansi.end(); ++d, ++str) {
        *d = (char)*str;
    }
    return ansi;
}

string ConvertWideStringToAnsiString(const wstring& str)
{
    string ansi(str.length(), L'\0');
    wstring::const_iterator s = str.begin();
    for (string::iterator d = ansi.begin(); d != ansi.end(); ++d, ++s) {
        *d = (char)*s;
    }
    return ansi;
}

// Splits a string
vector<string> Split(const string& str, const char* separators)
{
    vector<string> out;

    string::size_type start = 0;
    while ((start = str.find_first_not_of(separators, start)) != string::npos)
    {
        string::size_type end = str.find_first_of(separators, start);
        string::size_type len = (end != string::npos) ? end++ - start : string::npos;
        out.push_back(str.substr(start, len));
        start = end;
    }
    return out;
}

string  Uppercase(const string& str)  { return Uppercase(str.c_str()); }
string  Lowercase(const string& str)  { return Lowercase(str.c_str()); }
wstring Uppercase(const wstring& str) { return Uppercase(str.c_str()); }
wstring Lowercase(const wstring& str) { return Lowercase(str.c_str()); }

string Lowercase(const char* str)
{
    string result(str);
    transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}

string Uppercase(const char* str)
{
    string result(str);
    transform(result.begin(), result.end(), result.begin(), toupper);
    return result;
}

wstring Uppercase(const wchar_t* str)
{
    wstring result(str);
    transform(result.begin(), result.end(), result.begin(), toupper);
    return result;
}

wstring Lowercase(const wchar_t* str)
{
    wstring result(str);
    transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}

string Trim(const string& str, const char* delim)
{
    string::size_type start = str.find_first_not_of(delim);
    if (start != string::npos)
    {
        string::size_type end = str.find_last_not_of(delim);
        assert(end != string::npos);
        return str.substr(start, end - start + 1);
    }
    return "";
}

std::string Trim(const char* str, const char* delim)
{
    return Trim(string(str), delim);
}

string StripExtension(const string& str)
{
    string::size_type sl  = str.find_last_of("/\\");
    string::size_type dot = str.find_last_of(".");
    return (sl  == string::npos) ? str.substr(0, dot) : 
           (dot == string::npos) ? str.substr(sl + 1) :
           str.substr(sl + 1, dot - sl - 1);
}

string GetFilename(const string& path)
{
    string::size_type sl = path.find_last_of("\\/");
    return path.substr( (sl != string::npos) ? sl + 1 : 0 );
}

string GetBasePath(const string& path)
{
    string::size_type sl = path.find_last_of("\\/");
    return path.substr(0, (sl != string::npos) ? sl + 1 : 0 );
}

wstring GetFilename(const wstring& path)
{
    wstring::size_type sl = path.find_last_of(L"\\/");
    return path.substr( (sl != wstring::npos) ? sl + 1 : 0 );
}

wstring GetBasePath(const wstring& path)
{
    wstring::size_type sl = path.find_last_of(L"\\/");
    return path.substr(0, (sl != wstring::npos) ? sl + 1 : 0 );
}

}
