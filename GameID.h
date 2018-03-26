#ifndef GAMEID_H
#define GAMEID_H

typedef int GameID;
enum
{
    GID_NONE    = 0,
    GID_EAW     = 1,
    GID_EAW_FOC = 2,
    NUM_GAMES   = 2,
    GID_ALL     = GID_EAW | GID_EAW_FOC,
};

#endif