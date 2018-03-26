#ifndef ASSETS_MAPS_H
#define ASSETS_MAPS_H

#include "Assets/Files.h"
#include "GameID.h"
#include <vector>

namespace Assets {

class ChunkReader;

enum MapType
{
    MAP_LAND = 0,
    MAP_SPACE,
    NUM_MAP_TYPES
};

class Map
{
    class Loader_EaW;
    class Loader_FoC;
    friend class Loader_EaW;
    friend class Loader_FoC;
public:
    struct Track
    {
        std::string m_texture;
    };

    struct Object
    {
        unsigned long crc;              // CRC of the object type name
        unsigned long initialObject;    // Initial object that is placed on this object
    };

    struct Water
    {
	    std::string	maps[3];
    };

    struct Layer
    {
	    std::string	  colorTexture;
	    std::string	  normalTexture;
    };

    struct Environment
    {
        std::string m_scenario;
        std::string m_skybox1;
	    std::string m_skybox2;
        std::string m_clouds;
    };

    struct Properties
    {
        GameID        m_game;
        unsigned long m_numPlayers;
        MapType       m_type;
	    std::wstring  m_name;
	    std::wstring  m_planet;
    };

private:
    Properties               m_properties;
	Water				     m_water;
    std::vector<Layer>       m_layers;
	std::vector<Environment> m_environments;
    std::vector<Track>       m_tracks;
    std::vector<Object>      m_objects;
 
    void ReadMap_EaW(ChunkReader& reader, bool properties_only);
    void ReadMap_FoC(ChunkReader& reader, bool properties_only);
    void ReadMap(ChunkReader& reader, GameID game, bool properties_only);
public:
    const Properties&               GetProperties()   const { return m_properties; }
    const Water&                    GetWater()        const { return m_water; }
    const std::vector<Layer>&       GetLayers()       const { return m_layers; }
    const std::vector<Track>&       GetTracks()       const { return m_tracks; }
    const std::vector<Object>&      GetObjects()      const { return m_objects; }
    const std::vector<Environment>& GetEnvironments() const { return m_environments; }

    // Attempts to detect the Game ID from the map.
    // Returns GID_NONE if the file is not a valid or supported map.
    // The file pointer is restored after determining the game.
    static GameID DetectGameID(File& file);

    Map(File& file, bool properties_only = false);              // Loads a map, auto-detects game type
    Map(File& file, GameID game, bool properties_only = false); // Loads a map, of the specified game type
};

}

#endif