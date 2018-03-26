#include "Assets/ParticleSystem.h"
#include "General/Exceptions.h"
using namespace std;

namespace Assets
{

ParticleSystem::ParticleSystem(File& file)
{
    ChunkType   type;
    ChunkReader reader(file);

    Verify(reader.next() == 0x900);
	
    // Read name
    Verify(reader.next() == 0x0000);
    m_name = reader.readString();

    // Ignore this chunk
    Verify(reader.next() == 0x0001);

    // Read emitters
    Verify(reader.next() == 0x0800);
    while ((type = reader.next()) == 0x0700)
    {
        m_emitters.push_back(Emitter());
        Emitter& emitter = m_emitters.back();

	    emitter.normalTexture = "p_particle_depth_master.tga";

	    Verify(reader.next() == 0x02);
	    Verify(reader.next() == 0x03); emitter.colorTexture = reader.readString();
	    Verify(reader.next() == 0x16);
	    Verify(reader.next() == 0x29); reader.skip();
	    Verify(reader.next() == 0x01); reader.skip();

	    ChunkType type = reader.next();
	    if (type == 0x36) {
		    type = reader.next();
	    }

	    if (type == 0x45) {
		    emitter.normalTexture = reader.readString();
		    type = reader.next();
	    }
	    Verify(type == -1);
    }
    Verify(type == -1);

    // Read leave particles chunk
    type = reader.next();
    if (type == 0x0002)
    {
	    type = reader.next();
    }

    // End of 0900h chunk
    Verify(type == -1);
}

}