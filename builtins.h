#ifndef BUILTINS_H
#define BUILTINS_H

#include "GameID.h"

struct Builtin
{
    const char* name;
    GameID      games;
};

struct BuiltinInfo
{
    const Builtin* list;
    size_t         count;
};

namespace Builtins
{
    extern const BuiltinInfo HardpointTypes;
    extern const BuiltinInfo StoryTriggers;
    extern const BuiltinInfo StoryRewards;
    extern const BuiltinInfo Languages;
    extern const BuiltinInfo AbilityTypes;
    extern const BuiltinInfo ActivationStyles;
    extern const BuiltinInfo AbilityClasses;
    extern const BuiltinInfo TerrainTypes;
    extern const BuiltinInfo RenderModes;
    extern const BuiltinInfo WeatherTypes;
    extern const BuiltinInfo MousePointers;
    extern const BuiltinInfo VictoryConditions;
    extern const BuiltinInfo GameModes;
    extern const BuiltinInfo Difficulties;
    extern const BuiltinInfo AIGoalApplicationFlags;
}

#ifndef NDEBUG
extern void ValidateBuiltins();
#endif

#endif