#include "Assets/ChunkFile.h"
#include "General/Exceptions.h"
#include "General/ExactTypes.h"
#include <cassert>
#include <vector>
using namespace std;

namespace Assets {

#pragma pack(1)
struct CHUNKHDR
{
	uint32_t type;
	uint32_t size;
};

struct MINICHUNKHDR
{
	uint8_t type;
	uint8_t size;
};
#pragma pack()

ChunkType ChunkReader::nextMini()
{
	assert(m_curDepth >= 0);
	assert(m_size >= 0);

	if (m_miniSize >= 0)
	{
		// We're in a mini chunk, so skip it
		skip();
	}

    if (m_file.GetPosition() == m_offsets[m_curDepth])
	{
		// We're at the end of the current chunk, move up one
		m_curDepth--;
		m_size     = -1;
		m_position =  0;
		return -1;
	}

	MINICHUNKHDR hdr;
	if (m_file.Read((void*)&hdr, sizeof(MINICHUNKHDR)) != sizeof(MINICHUNKHDR))
	{
		throw ReadException();
	}

	m_miniSize   = letohl(hdr.size);
	m_miniOffset = m_file.GetPosition() + m_miniSize;
	m_position   = 0;

	return letohl(hdr.type);
}

ChunkType ChunkReader::next()
{
	assert(m_curDepth >= 0);

	if (m_size >= 0)
	{
		// We're in a data chunk, so skip it
		m_file.SetPosition(m_offsets[m_curDepth--]);
	}
	
	if (m_file.GetPosition() == m_offsets[m_curDepth])
	{
		// We're at the end of the current chunk, move up one
		m_curDepth--;
		m_size     = -1;
		m_position =  0;
		return -1;
	}

	CHUNKHDR hdr;
	if (m_file.Read((void*)&hdr, sizeof(CHUNKHDR)) != sizeof(CHUNKHDR))
	{
		throw ReadException();
	}

	unsigned long size = letohl(hdr.size);
	m_offsets[ ++m_curDepth ] = m_file.GetPosition() + (size & 0x7FFFFFFF);
	m_size     = (~size & 0x80000000) ? size : -1;
	m_miniSize = -1;
	m_position = 0;

	return letohl(hdr.type);
}

void ChunkReader::skip()
{
	if (m_miniSize >= 0)
	{
		m_file.SetPosition(m_miniOffset);
	}
	else
	{
		m_file.SetPosition(m_offsets[m_curDepth--]);
        m_size     = -1;
        m_position =  0;
	}
}

size_t ChunkReader::size()
{
	return (m_miniSize >= 0) ? m_miniSize : m_size;
}

string ChunkReader::readString()
{
	vector<char> data(size() / sizeof(char));
	read(&data[0], size());
	return &data[0];
}

wstring ChunkReader::readWideString()
{
	vector<wchar_t> data(size() / sizeof(wchar_t));
	read(&data[0], size());
	return &data[0];
}

unsigned char ChunkReader::readByte()
{
	uint8_t value;
    read(&value, sizeof(value));
	return value;
}

unsigned short ChunkReader::readShort()
{
	uint16_t value;
    read(&value, sizeof(value));
	return letohs(value);
}

unsigned long ChunkReader::readInteger()
{
	uint32_t value;
    read(&value, sizeof(value));
	return letohl(value);
}

size_t ChunkReader::read(void* buffer, size_t size, bool check)
{
	if (m_size >= 0)
	{
		size_t s = m_file.Read(buffer, min(m_position + (long)size, (long)this->size()) - m_position);
		m_position += (long)s;
		if (check && s != size)
		{
			throw ReadException();
		}
		return size;
	}
	throw ReadException();
}

ChunkReader::ChunkReader(File& file)
    : m_file(file)
{
    m_offsets[0] = (unsigned long)m_file.GetSize();
	m_curDepth   = 0;
	m_size       = -1;
	m_miniSize   = -1;
    m_file.AddRef();
    m_file.SetPosition(0);
}

ChunkReader::~ChunkReader()
{
    m_file.Release();
}

}