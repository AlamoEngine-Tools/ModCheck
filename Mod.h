#ifndef MOD_H
#define MOD_H

#include "Assets/Assets.h"
#include "Tags.h"
#include "builtins.h"
#include <iostream>
#include <list>
#include <map>
#include <set>
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

typedef std::string TextureName;
typedef std::string FactionName;

// An object ID is a unique identifier of any object
// in the entire mod. It is a simple combination of type
// and name.
// Do not confuse this with in-game objects. A texture is
// also an object.
struct ObjectID
{
    ObjType     type;
    std::string name;

    bool operator < (const ObjectID& rhs) const {
        return (type < rhs.type) || (type == rhs.type && name < rhs.name);
    }

    ObjectID(ObjType type, const std::string& name)
        : type(type), name(name) {}
};

// A reference is an object ID in a certain location.
struct Reference
{
    ObjectID id;
    Location location;

    bool operator <(const Reference& rhs) const {
        return (id < rhs.id) || (!(rhs.id < id) && location < rhs.location);
    }

    Reference(const ObjectID& id, const Location& loc)
        : id(id), location(loc) {}
};

// A list of references.
struct ReferenceList
{
    std::list<Reference> m_references;

    void add(const Location& location, const char*        value, const char* pattern, const char* prefix = NULL);
    void add(const Location& location, const std::string& value, const char* pattern, const char* prefix = NULL);
    void add(const Reference& ref);
    bool add(const XMLNode& node, const Tags& tags);
    void add(const BuiltinInfo& builtin, ObjType type, const Location& location, GameID game);
};

struct ModObject
{
    std::string   m_name;       // The name
    Location      m_location;   // Where it was declared
    ReferenceList m_references; // References in the object

    ModObject(const Location& loc) : m_location(loc) {}
};

typedef std::map<std::string, ModObject> ObjectList;
typedef std::set<std::string> DefinitionList;
typedef std::map<unsigned long, std::string> ChecksumMap;

class Mod
{
    std::auto_ptr<MegaTextureDirectory> m_mtd;
    std::auto_ptr<StringList> m_strings;

    GameID         m_game;

    DefinitionList m_damageTypes,
                   m_armorTypes,
                   m_surfaceFXTriggerTypes,
                   m_categories,
                   m_gameObjectProperties,
                   m_movementClasses,
                   m_aiGoalCategoryTypes;

    std::list<ReferenceList> m_demands;
    ReferenceList*           m_demand;

    ChecksumMap        m_checksums; // Checksums of m_gameObjects
    const ChecksumMap& m_reference; // Reference checksums of unmodded game objects

    ReferenceList m_globals;
    ObjectList    m_gameObjects,
                  m_radarMapEvents,
                  m_factions,
                  m_abilities,
                  m_sfxEvents,
                  m_surfaceFXs,
                  m_dynamicTracks,
                  m_terrainDecals,
                  m_lightningEffects,
                  m_shadowBlobs,
                  m_musicEvents,
                  m_speechEvents,
                  m_movies,
                  m_textCrawls,
                  m_tradeRoutes,
                  m_campaigns,
                  m_aiPlayers,
                  m_maps,
                  m_aiTemplates,
                  m_goalSets,
                  m_lensFlares,
                  m_goals,
                  m_equations,
                  m_commandbarComponents,
                  m_weatherScenarios,
                  m_hardpoints,
                  m_blackMarketItems,
                  m_difficulties,
                  m_tacticalCameras,
                  m_tradeRouteLines,
                  m_targetingPriorities,
                  m_mousePointers,
                  m_weatherModifiers,
                  m_heroClashes;

    typedef void       (Mod::*ObjectCallback)(const XMLNode&, ModObject&, const Tags&);
    typedef ModObject* (Mod::*FileCallback)(const XMLNode&, const char*, ObjectList&, const Tags&, const ObjectCallback& callback, bool allow_duplicates);
    typedef void       (Mod::*OnDemandCheck)(const Reference&, File& f);

    static int Lua_Require(lua_State *L);

    // Object-level callbacks
    void ParseCampaign(const XMLNode&, ModObject&, const Tags&);
    void ParseSurfaceFX(const XMLNode&, ModObject&, const Tags&);
    void ParseFaction(const XMLNode&, ModObject&, const Tags&);
    void ParseGameObject(const XMLNode&, ModObject&, const Tags&);
    void ParseReferences(const XMLNode&, ModObject&, const Tags&);
    void ParseTradeRouteLine(const XMLNode&, ModObject&, const Tags&);
    void ParseWeatherScenarioPhase(const XMLNode&, ModObject&, const Tags&);
    void ParseWeatherModifierModifier(const XMLNode&, ModObject&, const Tags&);

    // File-level callbacks
    ModObject* ParseWeatherModifier(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback = &Mod::ParseReferences, bool allow_duplicates = false);
    ModObject* ParseWeatherScenario(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback = &Mod::ParseReferences, bool allow_duplicates = false);
    ModObject* ParseObject         (const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback = &Mod::ParseReferences, bool allow_duplicates = false);
    ModObject* ParseGoal           (const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback = &Mod::ParseReferences, bool allow_duplicates = false);
    ModObject* ParseTemplate       (const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback = &Mod::ParseReferences, bool allow_duplicates = false);
    ModObject* ParseEquation       (const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback = &Mod::ParseReferences, bool allow_duplicates = false);

    void ParseModel     (const Location& location, Assets::Model& f);
    void ParseParticle  (const Location& location, Assets::ParticleSystem& f);

    // On-demand callbacks
    void ParseGoalSet   (const Reference& reference, File& f);
    void ParseStory     (const Reference& reference, File& f);
    void ParseStoryPlots(const Reference& reference, File& f);
    void CheckScript    (lua_State* s);
    void CheckScript    (const Reference& reference, File& f);
    void CheckModel     (const Reference& reference, File& f);
    void CheckParticle  (const Reference& reference, File& f);
    void CheckMap       (const Reference& reference, File& f);
    void CheckModelOrParticle(const Reference& reference, File& f);

    void ParseObjects     (const XMLNode& node,                       const char* type, ObjectList& objects, const Tags& tags, const FileCallback& callback = &Mod::ParseObject, const ObjectCallback& ocallback = &Mod::ParseReferences);
    void ParseFile        (const Location& loc, const char* filename, const char* type, ObjectList& objects, const Tags& tags, const FileCallback& callback = &Mod::ParseObject, const ObjectCallback& ocallback = &Mod::ParseReferences);
    void ParseIndexFile   (const Location& loc, const char* filename, const char* type, ObjectList& objects, const Tags& tags, const FileCallback& callback = &Mod::ParseObject, const ObjectCallback& ocallback = &Mod::ParseReferences);

    void ParseMarkup(const Location& location, const std::string& filename, ModObject& object);

    void ParseMap(const Location& location, const Assets::Map& map);
    void ParseAnimationSFXMaps(const Location& location, const char* filename);
    void ParseAIScript(const Location& location, const std::string& filename);
    void ParseGameConstants(const Location& location, const char* filename);
    void ParseAIPlayer(const Location& location, const std::string& filename);
    void ParseGoalFunctions(const Location& location, const std::string& filename);
    void ParseAudio(const Location& location, const char* filename);
    void ParseRadarMap(const Location& root, const char* filename);
    void ParseWeatherAudio(const Location& loc, const char* filename);
    void ParseEnumeration(const Location& location, const char* filename, const std::string& type, DefinitionList& definitions);

    void Load();
    void Validate();
public:
    // Assumes modpath is disabled and loads just the GameObjects to create a checksum reference
    // This will be used to give suggestions when a CRC miss occurs.
    static void LoadReferenceObjects(ChecksumMap& checksums);

    Mod(GameID game, const ChecksumMap& reference);
};

#endif