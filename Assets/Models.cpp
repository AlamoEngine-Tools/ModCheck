#include "Assets/Models.h"
#include "Assets/ChunkFile.h"
#include "General/Exceptions.h"
using namespace std;

namespace Assets {

/*
 * Parses a string like "MeshName_ALT1_LOD0" into its components.
 * It strips off the _ALT1 and _LOD0 parts and returns the clean name.
 */
static string ParseObjectName(string name)
{
    const struct {
        const char* name;
    } Patterns[] = {
        {"_ALT"},
        {"_LOD"},
        {NULL}
    };
    
    // See which patterns occur
    for (int i = 0; Patterns[i].name != NULL; i++)
    {
        string::size_type ofs = name.find(Patterns[i].name);
        if (ofs != string::npos)
        {
            // Parse the number that comes after the pattern
            string::size_type start = ofs + strlen(Patterns[i].name), c;
            for (c = start; c < name.size() && isdigit(name[c]); c++);
            if (c > start)
            {
                // Valid number, store it and remove the found pattern
                name.erase(ofs, c - ofs);
            }
        }
    }
    return name;
}

void Model::ReadSkeleton(ChunkReader& reader)
{
    Verify(reader.next() == 0x200);
    
    // Read bone count
    Verify(reader.next() == 0x201);
    m_bones.resize( reader.readInteger() );

    for (size_t i = 0; i < m_bones.size(); i++)
    {
        Verify(reader.next() == 0x202);
        
        // Read bone name
        Verify(reader.next() == 0x203);
        m_bones[i].m_name  = reader.readString();

        // Skip bone data
        ChunkType type = reader.next();
        Verify(type == 0x205 || type == 0x206);
        Verify(reader.next() == -1);
    }
    Verify(reader.next() == -1);
}

void Model::ReadMesh(ChunkReader& reader)
{
    Verify(reader.next() == 0x401);

    Verify(reader.next() == 0x402);
    unsigned long nMaterials = reader.readInteger();

    // Read the materials
    for (unsigned long i = 0; i < nMaterials; i++)
    {
        // This way an exception won't cause a memory leak
        m_materials.push_back(Material());
        Material& material = m_materials.back();

        // Read shader information
        Verify(reader.next() == 0x10100);
        Verify(reader.next() == 0x10101);
        material.m_name = reader.readString();
        
        ChunkType type;
        while ((type = reader.next()) != -1)
        {
            // Read shader parameter
            material.m_parameters.push_back(ShaderParameter());
            ShaderParameter& param = material.m_parameters.back();
            
            // Read parameter name
            Verify(reader.nextMini() == 1);

            // Read parameter value
            Verify(reader.nextMini() == 2);
            switch (type)
            {
                case 0x10102: param.m_type = PARAM_INT;     
                case 0x10103: param.m_type = PARAM_FLOAT;   
                case 0x10104: param.m_type = PARAM_COLOR;
                case 0x10106: param.m_type = PARAM_FLOAT4; break; 
                case 0x10105:
                    param.m_type = PARAM_TEXTURE;
                    param.m_texture = reader.readString();
                    break;

                default:
                    // Unsupported parameter type
                    throw BadFileException();
            }
            Verify(reader.nextMini() == -1);
        }
        Verify(reader.next() == 0x10000);
        Verify(reader.next() == 0x10001);
        Verify(reader.next() == 0x10002);
        type = reader.next();
        Verify(type == 0x10005 || type == 0x10007);
        Verify(reader.next() == 0x10004);
        if ((type = reader.next()) == 0x10006) {
            type = reader.next();
        }
        if (type == 0x1200) {
            reader.skip();
            type = reader.next();
        }
        Verify(type == -1);
    }

    Verify(reader.next() == -1);
}

void Model::ReadModel(ChunkReader& reader)
{
    // Read skeleton
    ReadSkeleton(reader);

    // Read meshes and lights
    ChunkType type;
    while(true)
    {
        if ((type = reader.next()) == 0x400) {
            ReadMesh(reader);
        } else if (type == 0x1300) {
            reader.skip();
        } else {
            break;
        }
    }

    if (type == 0x600)
    {
        // Read connections, proxies and dazzles
        Verify(reader.next() == 0x601);
        Verify(reader.nextMini() ==  1); unsigned long nObjects = reader.readInteger();
        Verify(reader.nextMini() ==  4); m_proxies.resize(reader.readInteger());

        for (size_t i = 0; i < nObjects; i++)
        {
            Verify(reader.next() == 0x602);
        }

        for (size_t i = 0; i < m_proxies.size(); i++)
        {
            Verify(reader.next() == 0x603);
            Verify(reader.nextMini() ==  5);
            m_proxies[i].m_name = ParseObjectName(reader.readString());
        }
        type = reader.next();
    }
    Verify(type == -1);
}

Model::Model(File& f)
{
    ChunkReader reader(f);
    ReadModel(reader);
}

}