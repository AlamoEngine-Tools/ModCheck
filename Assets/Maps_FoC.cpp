#include "Assets/Maps.h"
#include "Assets/ChunkFile.h"
#include "General/Exceptions.h"
using namespace std;

namespace Assets {

#include "Maps_Loading.h"

class Map::Loader_FoC
{
    Map&         m_map;
    ChunkReader& m_reader;

    void ReadProperties()
    {
	    Verify(m_reader.next() == 0);

        unsigned int type;
        BEGIN_PROPERTIES(m_reader)
        DEFINE_PROPERTY_INTEGER( 1, type)
        DEFINE_PROPERTY_INTEGER( 2, m_map.m_properties.m_numPlayers)
        DEFINE_PROPERTY_WSTRING( 8, m_map.m_properties.m_name)
        DEFINE_PROPERTY_WSTRING( 9, m_map.m_properties.m_planet)
        END_PROPERTIES

        static const struct {
            unsigned int value;
            MapType      type;
        } MapTypes[] = {
            {1, MAP_LAND},
            {2, MAP_SPACE},
            {0},
        };

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
            DEFINE_PROPERTY_STRING (41, env.m_scenario)
		    DEFINE_PROPERTY_STRING (25, env.m_skybox1)
		    DEFINE_PROPERTY_STRING (26, env.m_skybox2)
            DEFINE_PROPERTY_STRING (47, env.m_clouds)
            END_PROPERTIES
	    }
        Verify(type == 5);
        Verify(m_reader.size() == 0);     // Empty node, for some reason
	    Verify(m_reader.next() == -1);    // End of environment list

	    // We don't care about chunk type 2
	    Verify(m_reader.next() ==  2);
	    Verify(m_reader.next() ==  0);
	    Verify(m_reader.next() == -1);

        type = m_reader.next();
        if (type == 8)
        {
    	    // Read active environment index
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
        while ((type = m_reader.next()) == 0x10);
        Verify(type == -1);
    }

    void ReadTracks()
    {
        ChunkType type;
        while ((type = m_reader.next()) == 0x100)
        {
            m_map.m_tracks.push_back(Track());
            Track& track = m_map.m_tracks.back();

            Verify(m_reader.next() == 0x101); // Read track info
            Verify(m_reader.next() == 0x103); track.m_texture = m_reader.readString();
            Verify(m_reader.next() == 0x102); // Read track control points
            Verify(m_reader.next() == 0x104); // Read track widths
            Verify(m_reader.next() == 0x105); // Read track opacities
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

        size_t nLayers;
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
            Verify(m_reader.size() == 0);
            type = m_reader.next();
        }

        if (type == 0x4BA)
        {
            while ((type = m_reader.next()) == 0)
            {
            }
            Verify(type == -1);
            type = m_reader.next();
        }

        if (type == 0x4B7)
        {
            Verify(m_reader.next() == 0x4B8);
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }
        Verify(type == -1);
        
        type = m_reader.next();
        if (type == 0x46E)
        {
            Verify(m_reader.next() == 0);
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }

        if (type == 0x482)
        {
            Verify(m_reader.next() == 0);
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }
        
        if (type == 0x476)
        {
            Verify(m_reader.next() == 1);
            BEGIN_PROPERTIES(m_reader)
            DEFINE_PROPERTY_INTEGER(2, object.initialObject)
            END_PROPERTIES
            Verify(m_reader.next() == -1);
            type = m_reader.next();
        }

        if (type == 0x47a)
        {
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
    Loader_FoC(Map& map, ChunkReader& reader, bool properties_only)
        : m_map(map), m_reader(reader)
	{
        m_map.m_properties.m_game = GID_EAW_FOC;
        ReadProperties();

        if (!properties_only)
        {
            ChunkType type = m_reader.next();
            if (type == 3) type = m_reader.next();
            if (type == 2) {
                m_reader.skip();
                type = m_reader.next();
            }
            if (type == 19) {
                // Preview image, we don't care when loading
                m_reader.skip();
                type = m_reader.next();
            }
            Verify(type == 1);

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

void Map::ReadMap_FoC(ChunkReader& reader, bool properties_only)
{
    Loader_FoC loader(*this, reader, properties_only);
}

}