#ifndef ASSETS_ASSETS_H
#define ASSETS_ASSETS_H

#include <string>
#include <vector>

#include "Assets/Files.h"
#include "Assets/XML.h"
#include "Assets/MTD.h"
#include "Assets/Models.h"
#include "Assets/Maps.h"
#include "Assets/ParticleSystem.h"
#include "Assets/StringList.h"

namespace Assets
{
    // Initialize Assets subsystem with this list of search paths.
    void Initialize(const std::wstring& mod_path, const std::wstring& main_path, const std::wstring& old_path);
    void Uninitialize();

    //
    // Enumerate assets.
    //
    // Use the filter to enumerate assets in the search paths and MegaFile index.
    // The filter accepts ? and * as wildcards. e.g.: "Data\\Art\\Models\\EV_*.ALO"
    //
    // Returns NULL when no files have been found. Otherwise, GetFileName() and
    // GetFileSize() can be used until Next() returns false.
    //
    class IEnumerator : public Object
    {
    public:
        virtual bool               Next() = 0;
        virtual const std::string& GetFileName() const = 0;
        virtual size_t             GetFileSize() const = 0;
    };

    ptr<IEnumerator> Enumerate(const std::string& filter);

    //
    // The following functions load various asset types.
    // If they fail for I/O reasons (e.g., file not found, read error, bad file),
    // they return NULL. Other exceptions are passed through.
    //
    ptr<File> LoadFile(const std::string& filename);
    ptr<File> LoadTexture(const std::string& filename);
    ptr<File> LoadAnimation(const std::string& filename);
    ptr<File> LoadModelParticle(const std::string& filename);
    ptr<File> LoadMap(const std::string& filename);
    ptr<File> LoadShader(const std::string& filename);
    ptr<File> LoadXML(const std::string& filename);
    ptr<File> LoadSFX(const std::string& filename);
    ptr<File> LoadMusic(const std::string& filename);
    ptr<File> LoadSpeech(const std::string& filename);
    ptr<File> LoadScript(const std::string& filename);
    ptr<File> LoadCinematic(const std::string& filename);
}

#endif
