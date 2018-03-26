#ifndef TAGS_H
#define TAGS_H

#include <string>

#define WHITESPACE " \t\r\n\f\v"

typedef int ObjType;
enum
{
    OBJ_FLOAT                = 'f', // A floating point value
    OBJ_INTEGER              = 'i', // An integer value
    OBJ_BOOLEAN              = 'b', // A boolean value (True/Yes; False/No)

    OBJ_HARDPOINT_TYPE       = 'h', // A hardcoded hardpoint type (e.g., HARDPOINT_TYPE_ENGINE)
    OBJ_STORY_TRIGGER        = 'S', // A hardcoded story trigger (e.g., STORY_CONSTRUCT)
    OBJ_STORY_REWARD         = 'r', // A hardcoded story reward (e.g., CREDITS)
    OBJ_LANGUAGE             = 'l', // a hardcoded language (e.g., ENGLISH)
    OBJ_ABILITY_TYPE         = 'y', // A hardcoded ability type (e.g., DEFEND)
    OBJ_ACTIVATION_STYLE     = 'z', // A hardcoded activation style (e.g., USER_INPUT)
    OBJ_ABILITY_CLASS        = 'Z', // A hardcoded ability class (e.g., Income_Stream_Ability)
    OBJ_TERRAIN_TYPE         = 'j', // A hardcoded terrain type (e.g., URBAN)
    OBJ_RENDER_MODE          = 'g', // A hardcoded render mode (e.g., ADDITIVE)
    OBJ_WEATHER_TYPE         = 'Y', // A hardcoded weather type (e.g., SNOW)
    OBJ_VICTORY_CONDITION    = '7', // A hardcoded victory condition (e.g., Galactic_All_Planets_Controlled)
    OBJ_GAME_MODE            = '#', // A hardcoded game mode (e.g., SPACE)
    OBJ_DIFFICULTY           = '?', // A hardcoded difficulty (e.g., EASY)
    OBJ_AI_GOAL_FLAG         = '&', // A hardcoded AI Goal Application Flag (e.g., Friendly_Unit)

    OBJ_DAMAGE_TYPE          = 'd', // A damage type defined in GameConstants.xml
    OBJ_ARMOR_TYPE           = 'a', // An armor type defined in GameConstants.xml
    OBJ_CATEGORY             = 'C', // A game object category from Enum\GameObjectCategoryType.xml
    OBJ_MOVEMENT_CLASS       = 'c', // A movement class from Enum\MovementClassType.xml
    OBJ_OBJECT_PROPERTY      = '2', // A property from Enum\GameObjectPropertiesType.xml
    OBJ_GOAL_CATEGORY_TYPE   = '*', // An AI goal category type from Enum\AIGoalCategoryType.xml

    OBJ_FILE                 = 'n', // A file
    OBJ_TEXTURE              = 't', // A file in Data\Art\Textures
    OBJ_MODEL                = 'm', // A model file in Data\Art\Models
    OBJ_ANIMATION            = '%', // An animation in Data\Art\Models
    OBJ_MODEL_OR_PARTICLE    = '8', // A model or particle file in Data\Art\Models
    OBJ_PARTICLE             = 'p', // A particle file in Data\Art\Models
    OBJ_MAP                  = 'e', // A file in Data\Art\Maps
    OBJ_MTD_TEXTURE          = 'I', // A sub-file in Data\Art\Textures\MT_CommandBar.mtd
    OBJ_STRING               = 's', // A string in MasterTextFile.
    OBJ_SFX                  = 'x', // A file in Data\Audio\SFX
    OBJ_MUSIC                = 'u', // A file in Data\Audio\Music
    OBJ_SPEECH               = 'v', // A file in Data\Audio\Speech\<language>
    OBJ_CINEMATIC            = 'k', // A file in Data\Art\Cinematics
    OBJ_SCRIPT               = 'Q', // A file in Data\Scripts
    OBJ_XML                  = '4', // A file in Data\XML
    OBJ_SHADER               = '9', // A file in Data\Art\Shaders

    OBJ_GAME_OBJECT           = 'G', // An entry from a file from GameObjectFiles.xml
    OBJ_GAME_OBJECT_CRC       = '=', // CRC of an entry from a file from GameObjectFiles.xml
    OBJ_FACTION               = 'F', // An entry from a file from FactionFiles.xml
    OBJ_RADAR_MAP_EVENT       = 'R', // An entry from RadarMap.xml
    OBJ_ABILITY               = 'A', // A named ability in a game object
    OBJ_SFXEVENT              = 'X', // An entry from a file from SFXEventFiles.xml
    OBJ_LIGHTNING_EFFECT      = 'L', // An entry in LightningEffectTypes.xml
    OBJ_SHADOW_BLOB           = 'B', // An entry in ShadowBlobMaterials.xml
    OBJ_MUSIC_EVENT           = 'U', // An entry in MusicEvents.xml
    OBJ_SPEECH_EVENT          = 'V', // An entry in SpeechEvents.xml
    OBJ_MOVIE                 = 'M', // An entry in Movies.xml
    OBJ_TEXT_CRAWL            = 'W', // An entry in StarWars3DTextCrawl.xml
    OBJ_TERRAIN_DECAL         = 'H', // An entry in TerrainDecalFX.xml
    OBJ_SURFACE_FX            = 'o', // An entry in SurfaceFX.xml
    OBJ_DYNAMIC_TRACK         = 'q', // An entry in DynamicTrackFX.xml
    OBJ_TACTICAL_CAMERA       = 'T', // An entry in TacticalCameras.xml
    OBJ_TRADE_ROUTE           = 'O', // An entry in a file in TradeRouteFiles.xml
    OBJ_TRADE_ROUTE_LINE      = 'P', // An entry in TradeRouteLines.xml
    OBJ_HARDPOINT             = 'J', // An entry in a file in HardpointDataFiles.xml
    OBJ_DIFFICULTY_ADJUSTMENT = 'N', // An entry in DifficultyAdjustments.xml
    OBJ_TARGETING_PRIORITY    = 'w', // An entry in a file in TargetingPrioritySetFiles.xml
    OBJ_COMMANDBAR_COMPONENT  = 'E', // An entry in a file in CommandbarComponentFiles.xml
    OBJ_WEATHER_SCENARIO      = 'K', // An entry in WeatherScenarios.xml
    OBJ_WEATHER_MODIFIER      = '1', // An entry in WeatherModifiers.xml
    OBJ_MOUSE_POINTER         = '0', // An entry in a file in MousePointerFiles.xml
    OBJ_CAMPAIGN              = '3', // An entry in a file in CampaignFiles.xml
    OBJ_STORY                 = '5', // A Story_*.xml file in Data\XML
    OBJ_STORY_PLOT            = '6', // A Story_Plots_*.xml file in Data\XML
    OBJ_AI_PLAYER             = '@', // An entry in Data\XML\AI\Players\*.xml
    OBJ_AI_TEMPLATE           = '/', // An entry in Data\XML\AI\Templates\*.xml
    OBJ_GOAL_SET              = ':', // A file in Data\XML\AI\GoalFunctions
    OBJ_GOAL                  = '!', // An entry in Data\XML\AI\Goals\*.xml
    OBJ_EQUATION              = '$', // An entry in Data\XML\AI\PerceptualEquations\*.xml
    OBJ_BLACKMARKETITEM       = 'D', // An entry in Data\XML\BlackMarketItems.xml
    NUM_OBJ_TYPES
};

extern bool        IsObjOfType(const std::string& value, ObjType type);
extern const char* GetObjTypeName(ObjType type);

struct TagInfo
{
    const char* name;
    const char* pattern;
    const char* prefix;     // Prefix the value with this to form the reference
};

struct Tags
{
    const TagInfo* list;
    size_t         count;

    const TagInfo* find(const char* name) const;
};

extern const Tags Tags_None;
extern const Tags Tags_GameConstants;
extern const Tags Tags_SFXEvent;
extern const Tags Tags_GameObject;
extern const Tags Tags_MusicEvent;
extern const Tags Tags_ShadowBlob;
extern const Tags Tags_Faction;
extern const Tags Tags_SpeechEvent;
extern const Tags Tags_MusicEvent;
extern const Tags Tags_RadarMapEvent;
extern const Tags Tags_RadarMapSettings;
extern const Tags Tags_LightningEffect;
extern const Tags Tags_TextCrawl;
extern const Tags Tags_HeroClash;
extern const Tags Tags_Ability;
extern const Tags Tags_TerrainDecal;
extern const Tags Tags_SurfaceFX;
extern const Tags Tags_DynamicTrack;
extern const Tags Tags_WeatherAudio;
extern const Tags Tags_TacticalCamera;
extern const Tags Tags_TradeRoute;
extern const Tags Tags_TradeRouteLine;
extern const Tags Tags_Hardpoint;
extern const Tags Tags_Difficulty;
extern const Tags Tags_TargetingPriority;
extern const Tags Tags_CommandbarComponent;
extern const Tags Tags_WeatherScenario;
extern const Tags Tags_MousePointer;
extern const Tags Tags_WeatherModifier;
extern const Tags Tags_Audio;
extern const Tags Tags_Campaign;
extern const Tags Tags_Goal;
extern const Tags Tags_Template;
extern const Tags Tags_BlackMarketItem;
extern const Tags Tags_LensFlare;

#ifndef NDEBUG
extern void ValidateTags();
#endif

#endif