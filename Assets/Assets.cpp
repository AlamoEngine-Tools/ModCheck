/*
Asset Loader subsystem

Loads various types of assets by name. The core function is LoadFile which
loads any file in the virtual FS. It first looks through the physical search
paths for the file, before searching the file index constructed from the
MegaFiles listed in MegaFiles.xml. The latter is read during Initialization().

The Asset manager also keeps track of changed files, and notifies the resource
that was loaded from it when this has happened so it can reload itself.
*/
#include "Assets/Assets.h"
#include "Assets/FileIndex.h"
#include <algorithm>
#include <map>
#include <list>
using namespace std;

namespace Assets
{
// Base paths for various asset types.
// Note: They must end with a path delimiter!
static const char* BASE_PATH_CINEMATICS = "Data\\Art\\Cinematics\\";
static const char* BASE_PATH_MAPS       = "Data\\Art\\Maps\\";
static const char* BASE_PATH_ANIMATIONS = "Data\\Art\\Models\\";
static const char* BASE_PATH_MODELS     = "Data\\Art\\Models\\";
static const char* BASE_PATH_PARTICLES  = "Data\\Art\\Models\\";
static const char* BASE_PATH_TEXTURES   = "Data\\Art\\Textures\\";
static const char* BASE_PATH_SHADERS    = "Data\\Art\\Shaders\\";
static const char* BASE_PATH_SFX        = "Data\\Audio\\SFX\\";
static const char* BASE_PATH_MUSIC      = "Data\\Audio\\Music\\";
static const char* BASE_PATH_SPEECH     = "Data\\Audio\\Speech\\English\\";
static const char* BASE_PATH_SCRIPTS    = "Data\\Scripts\\";
static const char* BASE_PATH_XML        = "Data\\XML\\";

ptr<File> LoadXML(const std::string& filename)
{
    static const char* const extensions[] = {"xml", NULL};
    return LoadFile(BASE_PATH_XML + filename, extensions);
}

ptr<File> LoadTexture(const std::string& filename)
{
    static const char* const extensions[] = {"tga", "dds", NULL};
    return LoadFile(BASE_PATH_TEXTURES + filename, extensions);
}

ptr<File> LoadAnimation(const std::string& filename)
{
    static const char* const extensions[] = {"ala", NULL};
    return LoadFile(BASE_PATH_ANIMATIONS + filename, extensions);
}

ptr<File> LoadModelParticle(const std::string& filename)
{
    static const char* const extensions[] = {"alo", NULL};
    return LoadFile(BASE_PATH_MODELS + filename, extensions);
}

ptr<File> LoadShader(const std::string& filename)
{
    static const char* const extensions[] = {"fx", "fxo"};
    return LoadFile(BASE_PATH_SHADERS + filename, extensions);
}

ptr<File> LoadMap(const std::string& filename)
{
    static const char* const extensions[] = {"ted", NULL};
    return LoadFile(BASE_PATH_MAPS + filename, extensions);
}

ptr<File> LoadSFX(const std::string& filename)
{
    static const char* const extensions[] = {"wav", NULL};
    return LoadFile(BASE_PATH_SFX + filename, extensions);
}

ptr<File> LoadMusic(const std::string& filename)
{
    static const char* const extensions[] = {"mp3", NULL};
    return LoadFile(BASE_PATH_MUSIC + filename, extensions);
}

ptr<File> LoadSpeech(const std::string& filename)
{
    ptr<File> f;
    static const char* const extensions[] = {"mp3", NULL};
    if ((f = LoadFile(BASE_PATH_SPEECH + filename, extensions)) == NULL)
    {
        static const char* const extensions[] = {"wav", NULL};
        f = LoadFile(BASE_PATH_SFX + filename, extensions);
    }
    return f;
}

ptr<File> LoadScript(const std::string& filename)
{
    static const char* const extensions[] = {"lua", NULL};
    return LoadFile(BASE_PATH_SCRIPTS + filename, extensions);
}

ptr<File> LoadCinematic(const std::string& filename)
{
    static const char* const extensions[] = {"tec", NULL};
    return LoadFile(BASE_PATH_CINEMATICS + filename, extensions);
}

}