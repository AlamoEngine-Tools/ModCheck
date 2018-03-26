#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include <string>
#include <vector>

namespace Utils
{

// Computes CRC32 of a block of data
unsigned long CRC32(const void *data, size_t size);

// Converts an ANSI string to a Wide (Unicode) string
std::wstring ConvertAnsiStringToWideString(const char* str);
std::wstring ConvertAnsiStringToWideString(const std::string& str);

// Converts a Wide (Unicide) string to an ANSI string
std::string ConvertWideStringToAnsiString(const wchar_t* str);
std::string ConvertWideStringToAnsiString(const std::wstring& str);

// Uppercase and lowercase strings
std::string  Uppercase(const std::string& str);
std::string  Lowercase(const std::string& str);
std::wstring Uppercase(const std::wstring& str);
std::wstring Lowercase(const std::wstring& str);
std::string  Lowercase(const char* str);
std::string  Uppercase(const char* str);
std::wstring Uppercase(const wchar_t* str);
std::wstring Lowercase(const wchar_t* str);

std::string StripExtension(const std::string& str);

std::string Trim(const std::string& str, const char* delim = " \t\v\f\r\n");
std::string Trim(const char* str, const char* delim = " \t\v\f\r\n");

// Splits a string
std::vector<std::string> Split(const std::string& str, const char* separators);

std::string  GetFilename(const std::string& path);
std::string  GetBasePath(const std::string& path);
std::wstring GetFilename(const std::wstring& path);
std::wstring GetBasePath(const std::wstring& path);

}

#endif