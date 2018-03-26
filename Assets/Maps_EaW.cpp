#include "Assets/Maps.h"
#include "Assets/ChunkFile.h"
#include "General/Exceptions.h"
#include <cstdarg>
#include <cfloat>
using namespace std;

namespace Assets {

#include "Maps_Loading.h"

static const struct {
    unsigned int value;
    MapType      type;
} MapTypes[] = {
    {1, MAP_LAND},
    {2, MAP_SPACE},
    {0},
};

class Map::Loader_EaW
{
    Map&         m_map;
    ChunkReader& m_reader;

    void ReadProperties()
    {
	    Verify(m_reader.next() == 0);

        unsigned int type = 1;
        BEGIN_PROPERTIES(m_reader)
        DEFINE_PROPERTY_INTEGER( 1, type)
        DEFINE_PROPERTY_INTEGER( 2, m_map.m_properties.m_numPlayers)
        DEFINE_PROPERTY_WSTRING( 8, m_map.m_properties.m_name)
        DEFINE_PROPERTY_WSTRING( 9, m_map.m_properties.m_planet)
        END_PROPERTIES

        m_map.m_properties.m_type = MAP_LAND;
        for (size_t i = 0; MapTypes[i].value != 0; i++) {
            if (MapTypes[i].value == type) {
                m_map.m_properties.m_type = MapTypes[i].type;
                break;
            }
        }
    }

    void ReadEnvironments()
    {
        Verify(m_reader.next() == 0x100);
	    Verify(m_reader.next() == 4);

        ChunkType type;
        while ((type = m_reader.next()) == 6)
	    {
            // Load an environment
		    m_map.m_environments.push_back(Environment());
		    Environment& env = m_map.m_environments.back();

            BEGIN_PROPERTIES(m_reader)
            DEFINE_PROPERTY_STRING(41, env.m_scenario)
		    DEFINE_PROPERTY_STRING(25, env.m_skybox1)
		    DEFINE_PROPERTY_STRING(26, env.m_skybox2)
            DEFINE_PROPERTY_STRING(47, env.m_clouds)
            END_PROPERTIES
	    }

        Verify(type == 5);
        Verify(m_reader.size() == 0);     // Empty node, for some reason

	    Verify(m_reader.next() == -1);    // End of environment list

	    // We don't care about chunk type 2 (copy of the active environment fog settings)
	    Verify(m_reader.next() ==  2);
	    Verify(m_reader.next() ==  0);
	    Verify(m_reader.next() == -1);

        type = m_reader.next();
        if (type == 8)
        {
            type = m_reader.next();
        }
	    Verify(type == -1);
    }

    void ReadLayer(Layer& layer)
    {
	    Verify(m_reader.next() == 3);
        BEGIN_PROPERTIES(m_reader)
	    DEFINE_PROPERTY_STRING (12, layer.colorTexture)
	    DEFINE_PROPERTY_STRING (19, layer.normalTexture)
        END_PROPERTIES
    }

    void ReadDecals()
    {
        ChunkType type;
        while ((type = m_reader.next()) == 0x10)
        {
        }
        Verify(type == -1);
    }

    void ReadTracks()
    {
        ChunkType type;
        while ((type = m_reader.next()) == 0x100)
        {
            m_map.m_tracks.push_back(Track());
            Track& track = m_map.m_tracks.back();

            Verify(m_reader.next() == 0x101); // Track info
            Verify(m_reader.next() == 0x103); track.m_texture = m_reader.readString();
            Verify(m_reader.next() == 0x102); // Track control points
            Verify(m_reader.next() == 0x104); // Track widths
            Verify(m_reader.next() == 0x105); // Track opacities
            Verify(m_reader.next() == -1);
        }
        Verify(type == -1);
    }

    void ReadScrollInfo()
    {
        Verify(m_reader.next() == 0x0aa45bcd);
        Verify(m_reader.next() == -1);
    }

    void ReadTerrain()
    {
	    Verify(m_reader.next() == 0x101);
	    Verify(m_reader.next() == 0);

        size_t nLayers = 0;
        BEGIN_PROPERTIES(m_reader)
	    DEFINE_PROPERTY_INTEGER( 5, nLayers)
	    DEFINE_PROPERTY_STRING (29, m_map.m_water.maps[0])
	    DEFINE_PROPERTY_STRING (30, m_map.m_water.maps[1])
	    DEFINE_PROPERTY_STRING (31, m_map.m_water.maps[2])
        END_PROPERTIES

	    Verify(m_reader.next() == 7); ReadScrollInfo();
        Verify(m_reader.next() == 8); ReadScrollInfo();
        Verify(m_reader.next() == 5); // Read terrain vertices

	    // Read texture list
	    Verify(m_reader.next() == 2);
        m_map.m_layers.resize(nLayers);
	    for (size_t i = 0; i < m_map.m_layers.size(); i++)
	    {
		    ReadLayer(m_map.m_layers[i]);
	    }
	    Verify(m_reader.next() == -1);

	    Verify(m_reader.next() == 6);
        if (m_reader.size() > 0)
        {
            ReadDecals();
        }
    	
	    Verify(m_reader.next() == 9);
        if (m_reader.size() > 0)
        {
            ReadTracks();
        }

        Verify(m_reader.next() == -1);
    }

    void ReadPassability()
    {
        Verify(m_reader.next() == 0x10A);
    }

    void ReadObject()
    {
        // Read object info
        Verify(m_reader.next() == 0x454);       
        Verify(m_reader.next() == 0x459);
        Verify(m_reader.next() == 0x4B0);
        
        Object object;
        object.crc           = 0;
        object.initialObject = 0;

        BEGIN_PROPERTIES(m_reader)
        DEFINE_PROPERTY_INTEGER(1, object.crc)
        END_PROPERTIES

        ChunkType type = m_reader.next();
        if (type == 0x4B9)
        {
            // Unknown use; always empty
            Verify(m_reader.size() == 0);
            type = m_reader.next();
        }

        if (type == 0x4BA)
        {
            // Ability list
            while ((type = m_reader.next()) == 0);
            Verify(type == -1);
            type = m_reader.next();
        }

        if (type == 0x4B7)
        {
            // Unknown use
            Verify(m_reader.next() == 0x4B8);
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }
        Verify(type == -1);
        
        type = m_reader.next();
        if (type == 0x46E)
        {
            // Unknown use
            Verify(m_reader.next() == 0);
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }

        if (type == 0x482)
        {
            // Chunk that contains the hint text (even if not used)
            Verify(m_reader.next() == 0);
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }
        
        if (type == 0x476)
        {
            // Chunk that contains the CRC of the initial object (e.g, on build pads)
            Verify(m_reader.next() == 1);
            BEGIN_PROPERTIES(m_reader)
            DEFINE_PROPERTY_INTEGER(2, object.initialObject)
            END_PROPERTIES
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }

        if (type == 0x47a)
        {
            // Unknown use
            m_reader.skip();
            type = m_reader.next();
        }
        Verify(type == -1);
        m_map.m_objects.push_back(object);
    }

    void ReadObjects()
    {
        Verify(m_reader.next() == 0x102);
        Verify(m_reader.next() == 0);
        Verify(m_reader.next() == 1);
        if (m_reader.size() > 0)
        {
            ChunkType type;
            while ((type = m_reader.next()) != -1)
            {
                if (type == 0x44C) {
                    ReadObject();
                } else {
                    m_reader.skip();
                }
            }
        }
    }

public:
    Loader_EaW(Map& map, ChunkReader& reader, bool properties_only)
        : m_map(map), m_reader(reader)
    {
        m_map.m_properties.m_game = GID_EAW;
		ReadProperties();

        if (!properties_only)
        {
            Verify(m_reader.next() == 1);
            ReadEnvironments();
            if (m_map.m_properties.m_type == MAP_LAND)
            {
                ReadTerrain();
                ReadPassability();
            }
            ReadObjects();
        }
    }
};

void Map::ReadMap_EaW(ChunkReader& reader, bool properties_only)
{
    Loader_EaW loader(*this, reader, properties_only);
}

}