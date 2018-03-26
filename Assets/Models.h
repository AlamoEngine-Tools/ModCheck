#ifndef ASSETS_MODELS_H
#define ASSETS_MODELS_H

#include "Assets/Assets.h"
#include "General/ExactTypes.h"

enum ParameterType
{
    PARAM_INT = 0,
    PARAM_FLOAT,
    PARAM_FLOAT3,
    PARAM_FLOAT4,
    PARAM_COLOR,
    PARAM_TEXTURE,
    NUM_PARAMETER_TYPES
};

struct ShaderParameter
{
    ParameterType m_type;
    std::string   m_texture;
};

namespace Assets {

class ChunkReader;

class Model : public Object
{
public:
    struct Bone
    {
        std::string m_name;
    };

    struct Material
    {
        std::string                  m_name;
        std::vector<ShaderParameter> m_parameters;
    };

    struct Proxy
    {
        std::string m_name;
    };

private:
    std::vector<Bone>     m_bones;
    std::vector<Material> m_materials;
    std::vector<Proxy>    m_proxies;

    void ReadModel(ChunkReader& reader);
    void ReadSkeleton(ChunkReader& reader);
    void ReadMesh(ChunkReader& reader);
public:
    size_t GetNumBones()     const { return m_bones.size(); }
    size_t GetNumMaterials() const { return m_materials.size(); }
    size_t GetNumProxies()   const { return m_proxies.size(); }

    const Bone&     GetBone(size_t i)     const { return m_bones[i]; }
    const Material& GetMaterial(size_t i) const { return m_materials[i]; }
    const Proxy&    GetProxy(size_t i)    const { return m_proxies[i]; }

    Model(File& f);
};

}

#endif