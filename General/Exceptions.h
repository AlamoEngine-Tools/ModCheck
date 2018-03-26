#ifndef GENERAL_EXCEPTIONS_H
#define GENERAL_EXCEPTIONS_H

#include <string>

class wexception
{
    std::wstring m_message;

public:
    const wchar_t* what() const { return m_message.c_str(); } 
    wexception(const std::wstring& message) : m_message(message) {}
};

class wruntime_error : public wexception
{
public:
    wruntime_error(const std::wstring& message) : wexception(message) {}
};

class ParseException : public std::runtime_error
{
public:
    ParseException(const std::string& message = "Parse error") : std::runtime_error(message) {}
};

class IOException : public wruntime_error
{
public:
    IOException(const std::wstring& message = L"I/O Exception") : wruntime_error(message) {}
};

class BadFileException : public IOException
{
public:
    BadFileException() : IOException(L"Bad or corrupted file") {}
};

class ReadException : public IOException
{
public:
    ReadException() : IOException(L"Cannot read file") {}
};

class FileNotFoundException : public IOException
{
public:
    FileNotFoundException(const std::wstring& path) : IOException(L"File not found: " + path) {}
};

#ifndef NDEBUG
// Debug Verify. Prints which verification failed.
static inline void __Verify(bool expr, const char* filename, int line) {
	if (!expr) {
        fprintf(stderr, "Bad file at %s:%d\n", filename, line);
		throw BadFileException();
	}
}
#define Verify(expr) __Verify(expr, __FILE__, __LINE__)
#else
// Release Verify. Just throws the exception.
static void inline Verify(bool expr) {
	if (!expr) {
		throw BadFileException();
	}
}
#endif

#endif