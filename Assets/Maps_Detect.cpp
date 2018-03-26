#include "Assets/Maps.h"
#include "Assets/ChunkFile.h"
#include "General/Exceptions.h"

namespace Assets {

static GameID DetectGameID_(ChunkReader& reader)
{
    // Check properties
    Verify(reader.next() == 0);
    Verify(reader.nextMini() == 0);
    Verify(reader.nextMini() == 1);
    Verify(reader.nextMini() == 2);
    Verify(reader.nextMini() == 3);
    Verify(reader.nextMini() == 4);
    Verify(reader.nextMini() == 5);
    Verify(reader.nextMini() == 6);
    Verify(reader.nextMini() == 7);
    ChunkType type = reader.nextMini();
    if (type == 11) type = reader.nextMini();
    if (type ==  8) type = reader.nextMini();
    if (type ==  9) type = reader.nextMini();
    if (type == 10) type = reader.nextMini();
    if (type == 16) return GID_EAW_FOC; // Optional for EAW_FOC

    type = reader.next();
    if (type ==  3) return GID_EAW_FOC; // Optional for EAW_FOC
    if (type ==  2) return GID_EAW_FOC; // Optional for EAW_FOC
    if (type == 19) return GID_EAW_FOC; // Optional for EAW_FOC

    // Assume we're EAW
    return GID_EAW;
}

// Attempts to detect the Game ID from the map.
// Returns GID_NONE if the file is not a valid or supported map.
// The file pointer is restored after determining the game.
GameID Map::DetectGameID(File& file)
{
    GameID game = GID_NONE;
    size_t original_position = file.GetPosition();
    try
    {
        ChunkReader reader(file);
        game = DetectGameID_(reader);
    }
    catch (IOException)
    {
        // Ignore I/O errors
    }
    file.SetPosition(original_position);
    return game;
}

}
