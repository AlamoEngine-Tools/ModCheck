#include "Assets/Maps.h"
#include "Assets/ChunkFile.h"
#include "General/Exceptions.h"
using namespace std;

namespace Assets {

void Map::ReadMap(ChunkReader& reader, GameID game, bool properties_only)
{
    switch (game)
    {
    case GID_EAW:       ReadMap_EaW(reader, properties_only); break;
    case GID_EAW_FOC:   ReadMap_FoC(reader, properties_only); break;
    default:            throw BadFileException(); break;
    }
}

Map::Map(File& file, bool properties_only)
{
    GameID game = DetectGameID(file);
    ChunkReader reader(file);
    ReadMap(reader, game, properties_only);
}

Map::Map(File& file, GameID game, bool properties_only)
{
    ChunkReader reader(file);
    ReadMap(reader, game, properties_only);
}

}
