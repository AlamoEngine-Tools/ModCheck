#ifndef ASSETS_PARTICLESYSTEM_H
#define ASSETS_PARTICLESYSTEM_H

#include "Assets/ChunkFile.h"
#include <string>
#include <vector>
#include <set>

namespace Assets
{

class ParticleSystem
{
public:
	struct Emitter
	{
		std::string colorTexture;
		std::string normalTexture;
	};

	const std::string& GetName()                const { return m_name; }
    size_t             GetNumEmitters()         const { return m_emitters.size(); }
    const Emitter&     GetEmitter(size_t index) const { return m_emitters[index]; }

    ParticleSystem(File& file);
	
private:
	std::string          m_name;
	std::vector<Emitter> m_emitters;
};

}
#endif