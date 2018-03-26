#ifndef ASSETS_CHUNKFILE_H
#define ASSETS_CHUNKFILE_H

#include "Assets/Files.h"
#include <string>
#include <utility>

namespace Assets {

typedef long ChunkType;

struct ChunkHeader
{
	unsigned long type;
	unsigned long size;
};

struct MiniChunkHeader
{
	unsigned char type;
	unsigned char size;
};

class ChunkReader
{
	static const int MAX_CHUNK_DEPTH = 256;

	File&  m_file;
	long   m_position;
	long   m_size;
	size_t m_offsets[ MAX_CHUNK_DEPTH ];
	long   m_miniSize;
	size_t m_miniOffset;
	int    m_curDepth;

public:
	ChunkType   next();
	ChunkType   nextMini();
	void        skip();
	size_t      size();
	size_t      read(void* buffer, size_t size, bool check = true);
    size_t      tell() { return m_position; }
    bool        group() const { return m_size < 0; }

	float			readFloat();
	unsigned char   readByte();
	unsigned short	readShort();
	unsigned long	readInteger();
	std::string		readString();
	std::wstring	readWideString();

	ChunkReader(File& file);
    ~ChunkReader();
};

//
// TemplateReader is a class to allow for the reading
// of values from ChunkReaders by templates.
//
template <typename T> 
struct TemplateReader {
    static T read(ChunkReader& reader);
};
template <> struct TemplateReader<char>         { static char         read(ChunkReader& reader) { return reader.readByte();    } };
template <> struct TemplateReader<float>        { static float        read(ChunkReader& reader) { return reader.readFloat();      } };
template <> struct TemplateReader<int>          { static int          read(ChunkReader& reader) { return reader.readInteger();    } };
template <> struct TemplateReader<std::string>  { static std::string  read(ChunkReader& reader) { return reader.readString();     } };
template <> struct TemplateReader<std::wstring> { static std::wstring read(ChunkReader& reader) { return reader.readWideString(); } };

template <typename T1, typename T2> 
struct TemplateReader< std::pair<T1,T2> > {
    static std::pair<T1,T2> read(ChunkReader& reader) {
        const T1 t1 = TemplateReader<T1>::read(reader);
        const T2 t2 = TemplateReader<T2>::read(reader);
        return std::make_pair(t1,t2);
    }
};

}
#endif