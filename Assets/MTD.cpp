#include "MTD.h"
#include "General/ExactTypes.h"
#include "General/Utils.h"
#include "General/Exceptions.h"
using namespace std;

#pragma pack(1)
struct FileInfo
{
    char name[64];
    uint32_t x, y, w, h;
    unsigned char alpha;
};
#pragma pack()

bool MegaTextureDirectory::exists(const string& filename) const
{
    string name = Utils::Uppercase(filename);
    return m_files.find(name) != m_files.end();
}

MegaTextureDirectory::MegaTextureDirectory(const ptr<File> f)
{
    uint32_t leCount;
    if (f->Read(&leCount, sizeof leCount) != sizeof leCount) {
        throw ReadException();
    }
    unsigned long count = letohl(leCount);
    for (unsigned long i = 0; i < count; i++)
    {
        FileInfo fi;
        if (f->Read(&fi, sizeof fi) != sizeof fi) {
            throw ReadException();
        }
        m_files.insert(fi.name);
    }
}
