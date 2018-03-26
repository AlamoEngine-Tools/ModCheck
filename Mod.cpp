#include "Mod.h"
#include "Assets/Assets.h"
#include "General/Utils.h"
#include "General/Exceptions.h"
#include <cassert>
#include <queue>
#include <sstream>
#include <windows.h>
using namespace std;

static void error(const Location& loc, const string& message)
{
    if (!loc.filename.empty()) {
        cerr << loc.filename;
        if (loc.line > 0) {
            cerr << ":" << loc.line;
        }
        cerr << ": ";
    }
    cerr << message << endl;
}

static void unknown(const Location& loc, const char* type, const string& name)
{
    error(loc, string("unknown ") + type + " \"" + name + "\"");
}

static void unknown_crc(const Location& loc, unsigned long crc, const ChecksumMap& reference)
{
    stringstream ss;
    ss << "unknown GameObject CRC: 0x";
    ss.width(8);
    ss.fill('0');
    ss.setf(ios::hex, ios::basefield);
    ss << crc;
    ChecksumMap::const_iterator p = reference.find(crc);
    if (p != reference.end()) {
        ss << " (could be \"" << p->second << "\")";
    }
    error(loc, ss.str());
}


static void duplicate(const Location& loc, const string& type, const string& name, const Location& previous)
{
    error(loc, "duplicate " + type + " found: \"" + name + "\"");
    error(previous, "previous declaration was here");
}

static bool isident(int c)
{
    return isalnum(c) || c == '_' || c == '.' || c == '\\' || c == '/' || c == '-';
}

static ptr<File> LoadAsset(const Reference& ref, bool error = true)
{
    ptr<File> (*loader)(const string&) = NULL;
    switch (ref.id.type)
    {
        case OBJ_FILE:      loader = &Assets::LoadFile; break;
        case OBJ_TEXTURE:   loader = &Assets::LoadTexture; break;
        case OBJ_SFX:       loader = &Assets::LoadSFX; break;
        case OBJ_MUSIC:     loader = &Assets::LoadMusic; break;
        case OBJ_SPEECH:    loader = &Assets::LoadSpeech; break;
        case OBJ_CINEMATIC: loader = &Assets::LoadCinematic; break;
        case OBJ_MAP:       loader = &Assets::LoadMap; break;
        case OBJ_ANIMATION: loader = &Assets::LoadAnimation; break;
        case OBJ_MODEL_OR_PARTICLE:
        case OBJ_PARTICLE:
        case OBJ_MODEL:     loader = &Assets::LoadModelParticle; break;
        case OBJ_SCRIPT:    loader = &Assets::LoadScript; break;
        case OBJ_SHADER:    loader = &Assets::LoadShader; break;
        case OBJ_STORY_PLOT:
        case OBJ_STORY:
        case OBJ_GOAL_SET:
        case OBJ_XML:       loader = &Assets::LoadXML; break;
        default:
            assert(0);
            return NULL;
    }
    ptr<File> f = loader(ref.id.name);
    if (f == NULL && error)
    {
        unknown(ref.location, GetObjTypeName(ref.id.type), ref.id.name);
    }
    return f;
}

static bool CheckMTD(const Reference& ref, auto_ptr<MegaTextureDirectory>& mtd)
{
    return (mtd.get() != NULL && mtd->exists(ref.id.name));
}

static bool CheckString(const Reference& ref, auto_ptr<StringList>& strings)
{
    return (strings.get() != NULL && strings->exists(ref.id.name));
}

static bool CheckDefinition(const Reference& ref, DefinitionList& definitions)
{
    return definitions.find(Utils::Uppercase(ref.id.name)) != definitions.end();
}

static bool CheckBuiltin(const Reference& ref, const BuiltinInfo& info, GameID game, bool error = true)
{
    // Do a binary search on the builtin list
    int low = 0, high = (int)info.count - 1;
    while (high >= low)
    {
        int mid = (low + high) / 2;
        const char* key = info.list[mid].name;
        int cmp = _stricmp(ref.id.name.c_str(), key);
        if (cmp == 0)
        {
            return (info.list[mid].games & game) != 0;
        }
        if (cmp < 0) high = mid - 1;
        else         low  = mid + 1;
    }
    if (error)
    {
        unknown(ref.location, GetObjTypeName(ref.id.type), ref.id.name);
    }
    return false;
}

static const ModObject* FindObject(const Reference& ref, const ObjectList& objects)
{
    ObjectList::const_iterator p = objects.find(Utils::Uppercase(ref.id.name));
    if (p != objects.end()) {
        // Found
        return &p->second;
    }
    return NULL;
}

void ReferenceList::add(const Location& location, const std::string& value, const char* pattern, const char* prefix)
{
    add(location, value.c_str(), pattern, prefix);
}

void ReferenceList::add(const Location& location, const char* value, const char* pattern, const char* prefix)
{
    // Attempt to match the pattern
    const char *loop = NULL, *s = pattern;
    bool single = (strlen(pattern) == 1);
    
    // Loop over all tokens in data
    for (const char *d = value; *d != '\0'; ++s)
    {
        // Find the start of the first token
        while (isspace(*d)) d++;
        if (*d != '\0')
        {
            // Find the end of the token
            const char* end;
            if (single) {
                // Single value; trim right-side of entire string
                for (end = d + strlen(d); end > d && isspace(*(end-1)); end--);
            } else {
                // Multiple values, find the end of the identifier
                for (end = d; isident(*end); end++);
            }
            string value(d, end - d);

            while (*s == ')') {
                // Move back to loop start
                assert(loop != NULL);
                s = loop;
                loop = NULL;
            }
            while (*s == '(') {
                // Set loop start
                assert(loop == NULL);
                loop = s++;
            }

            if (*s == '\0') {
                error(location, "unexpected value: \"" + value + "\"");
                break;
            }

            if (!value.empty())
            {
                if (!IsObjOfType(value, *s)) {
                    error(location, "expected " + string(GetObjTypeName(*s)) + " as value");
                } else {
                    if (prefix != NULL) {
                        // Prefix value to form reference
                        value = prefix + value;
                    }
                    m_references.push_back(Reference(ObjectID(*s, value), location));
                }
            }

            d = end;
            if (*end != '\0')
            {
                // Find separator after identifier
                const char* sep = end;
                while (isspace(*sep)) sep++;
                d = (!isident(*sep) ? sep : end) + 1;
            }
        }
    }
}

// See if the node tag occurs in the tag list, and if so,
// add it's value as a reference.
// Returns true if the node's tag was found in the list.
bool ReferenceList::add(const XMLNode& node, const Tags& tags)
{
    // Find the tag
    const TagInfo* ti = tags.find(node.GetName());
    if (ti == NULL)
    {
        // Not found
        return false;
    }

    const char* data = node.GetData();
    if (node.begin() != node.end())
    {
        // Node has children nodes
        error(node, "node should have text; not child nodes");
    }
    else if (ti->pattern != NULL && data != NULL)
    {
        add(node, data, ti->pattern, ti->prefix);
    }
    return true;
}

void ReferenceList::add(const Reference& ref)
{
    m_references.push_back(ref);
}

void ReferenceList::add(const BuiltinInfo& builtin, const ObjType type, const Location& location, GameID game)
{
    for (size_t i = 0; i < builtin.count; i++)
    {
        if (builtin.list[i].games & game)
        {
            m_references.push_back(Reference(ObjectID(type, builtin.list[i].name), location));
        }
    }
}

// Default tag handler; just try to add it as reference
void Mod::ParseReferences(const XMLNode& node, ModObject& object, const Tags& tags)
{
    object.m_references.add(node, tags);
}

void Mod::ParseSurfaceFX(const XMLNode& node, ModObject& object, const Tags& tags)
{
    if (node.Equals("Surface_Settings"))
    {
        for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
        {
            ParseReferences(*p, object, tags);
        }
    }
}

void Mod::ParseMarkup(const Location& location, const string& filename, ModObject& object)
{
    ptr<File> f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), location));
    if (f == NULL) try
    {
        XMLTree tree(*f);
        const XMLNode& root = tree.GetRoot();
        for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
        {
            if (p->Equals("Planets"))
            {
                const char* hintset = p->GetAttribute("HintSet");
                if (hintset == NULL || _stricmp(hintset, "galactic_hints") != 0)
                {
                    error(*p, "expected 'HintSet=\"GalacticHints\"' attribute");
                    continue;
                }
                
                // Even node is a planet (GameObject)
                for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
                {
                    object.m_references.add(Reference(ObjectID(OBJ_GAME_OBJECT, q->GetName()), *q));
                }
            }
        }
    }
    catch (ParseException& e)
    {
        error(location, e.what());
    }
}

void Mod::ParseMap(const Location& location, const Assets::Map& map)
{
    const Assets::Map::Properties& properties = map.GetProperties();
    if (properties.m_name.compare(0, 5, L"TEXT_") == 0) {
        // The name starts with TEXT_, so it's probably a text string reference
        m_demand->add(location, Utils::ConvertWideStringToAnsiString(properties.m_name), "s");
    }

    for (size_t i = 0; i < 3; i++) {
        m_demand->add(location, map.GetWater().maps[i], "t");
    }
    
    const vector<Assets::Map::Environment>& environments = map.GetEnvironments();
    for (size_t i = 0; i < environments.size(); i++)
    {
        m_demand->add(location, environments[i].m_clouds,   "t");
        m_demand->add(location, environments[i].m_skybox1,  "G");
        m_demand->add(location, environments[i].m_skybox2,  "G");
        m_demand->add(location, environments[i].m_scenario, "K");
    }

    const vector<Assets::Map::Layer>& layers = map.GetLayers();
    for (size_t i = 0; i < layers.size(); i++)
    {
        m_demand->add(location, layers[i].colorTexture,  "t");
        m_demand->add(location, layers[i].normalTexture, "t");
    }

    const vector<Assets::Map::Track>& tracks = map.GetTracks();
    for (size_t i = 0; i < tracks.size(); i++)
    {
        m_demand->add(location, tracks[i].m_texture, "t");
    }

    set<unsigned long> crcs;
    const vector<Assets::Map::Object>& objects = map.GetObjects();
    for (size_t i = 0; i < objects.size(); i++)
    {
        crcs.insert(objects[i].crc);
        if (objects[i].initialObject != 0) {
            crcs.insert(objects[i].initialObject);
        }
    }

    for (set<unsigned long>::const_iterator c = crcs.begin(); c != crcs.end(); ++c)
    {
        stringstream ss;
        ss << *c;
        m_demand->add(Reference(ObjectID(OBJ_GAME_OBJECT_CRC, ss.str()), location));
    }
}

void Mod::ParseCampaign(const XMLNode& node, ModObject& object, const Tags& tags)
{
    if (node.Equals("Markup_Filename"))
    {
        const char* data = node.GetData();
        if (data != NULL)
        {
            vector<string> refs = Utils::Split(data, ",");
            if (refs.size() > 0) {
                object.m_references.add(Reference(ObjectID(OBJ_FACTION, Utils::Trim(refs[0])), node));
            }
            if (refs.size() > 1) {
                ParseMarkup(node, "AI\\GalacticMarkup\\" + Utils::Trim(refs[1]), object);
            }
        }
        return;
    }
    return ParseReferences(node, object, tags);
}

void Mod::ParseModel(const Location& location, Assets::Model& model)
{
    set<string> shaders, textures, particles;
    
    for (size_t i = 0; i < model.GetNumMaterials(); i++)
    {
        const Assets::Model::Material& material = model.GetMaterial(i);
        if (_stricmp(material.m_name.c_str(), "aldefault.fx") != 0 &&
            _stricmp(material.m_name.c_str(), "default.fx") != 0)
        {
            // Don't check for alDefault.fx and Default.fx
            shaders.insert(material.m_name);
        }

        for (size_t j = 0; j < material.m_parameters.size(); j++)
        {
            if (material.m_parameters[j].m_type == PARAM_TEXTURE)
            {
                textures.insert(material.m_parameters[j].m_texture);
            }
        }
    }
    
    for (size_t i = 0; i < model.GetNumProxies(); i++)
    {
        const Assets::Model::Proxy& proxy = model.GetProxy(i);
        particles.insert(proxy.m_name);
    }

    for (set<string>::const_iterator p = shaders  .begin(); p != shaders  .end(); ++p) m_demand->add(Reference(ObjectID(OBJ_SHADER,   *p), location));
    for (set<string>::const_iterator p = textures .begin(); p != textures .end(); ++p) m_demand->add(Reference(ObjectID(OBJ_TEXTURE,  *p), location));
    for (set<string>::const_iterator p = particles.begin(); p != particles.end(); ++p) m_demand->add(Reference(ObjectID(OBJ_PARTICLE, *p), location));
}

void Mod::ParseParticle(const Location& location, Assets::ParticleSystem& particle)
{
    string filename = Utils::GetFilename(location.filename);
    string::size_type ext = filename.find_last_of(".");
    if (ext != string::npos) filename.erase(ext);
    if (_stricmp(filename.c_str(), particle.GetName().c_str()) != 0)
    {
        error(location, "particle's internal name does not match its filename: " + location.filename + ";" + filename + ";" + particle.GetName());
    }
    
    set<string> textures;
    for (size_t i = 0; i < particle.GetNumEmitters(); i++)
    {
        const Assets::ParticleSystem::Emitter& emitter = particle.GetEmitter(i);
        textures.insert(Utils::GetFilename(emitter.normalTexture));
        textures.insert(Utils::GetFilename(emitter.colorTexture));
    }

    for (set<string>::const_iterator p = textures .begin(); p != textures .end(); ++p)
    {
        m_demand->add(Reference(ObjectID(OBJ_TEXTURE,  *p), location));
    }
}

void Mod::CheckMap(const Reference& reference, File& f)
{
    try
    {
        Assets::Map map(f);
        ParseMap(Location(f.GetName()), map);
    }
    catch (BadFileException&)
    {
        error(reference.location, "Bad map file: " + f.GetName());
    }
}

void Mod::CheckModel(const Reference& reference, File& f)
{
    try
    {
        Assets::Model model(f);
        ParseModel(Location(f.GetName()), model);
    }
    catch (BadFileException&)
    {
        error(reference.location, "Bad model file: " + f.GetName());
    }
}

void Mod::CheckModelOrParticle(const Reference& reference, File& f)
{
    Location location(f.GetName());
    try
    {
        Assets::Model model(f);
        ParseModel(location, model);
    }
    catch (BadFileException&)
    {
        try
        {
            Assets::ParticleSystem particle(f);
            ParseParticle(location, particle);
        }
        catch (BadFileException&)
        {
           error(reference.location, "Bad model or particle file: " + f.GetName());
        }
    }
}

void Mod::CheckParticle(const Reference& reference, File& f)
{
    try
    {
        Assets::ParticleSystem particle(f);
        ParseParticle(Location(f.GetName()), particle);
    }
    catch (BadFileException&)
    {
        error(reference.location, "Bad model or particle file: " + f.GetName());
    }
}

struct LuaLoadInfo
{
    static const size_t BUFFER_SIZE = 32*1024;

    File* f;
    char  buffer[BUFFER_SIZE];
};

static const char * LuaFileReader(lua_State *s, void *data, size_t *size)
{
    LuaLoadInfo* lli = (LuaLoadInfo*)data;
    *size = 0;
    if (!lli->f->IsEOF()) try {
        *size = lli->f->Read(lli->buffer, LuaLoadInfo::BUFFER_SIZE);
    } catch (IOException&) {
    }
    return (*size > 0) ? lli->buffer : NULL;
}

static void lua_error(const string& filename, lua_State* L)
{
    int line = -1;
    const char* lua_error = lua_tostring(L, -1);
    const char* col = strchr(lua_error, ':');
    if (col++ != NULL) {
        sscanf(col, "%d", &line);
        // Advance past the next ": " to get the actual error message
        if ((col = strchr(col, ':'))++ != NULL) {
            lua_error = isspace(*col) ? col + 1 : col;
        }
    }
    // We take our own filename, and append Lua's error message
    error(Location(filename, line), lua_error);
}

struct LuaInfo
{
    Mod*    mod;
    string* basepath;
    File*   f;
};

// A dummy function that will be called for every operation on
// every object in a Lua script. This way we can execute Lua scripts
// and extract specific meaningful data without having to implement
// every single type and function from the game.
static int Lua_Dummy(lua_State *L)
{
    static const char* MetaMethods[] = {
        "__add",
        "__sub",
        "__mul",
        "__div",
        "__pow",
        "__unm",
        "__concat",
        "__eq",
        "__lt",
        "__le",
        "__index",
        //"__newindex", // Not this one, we do want to be able to regularly set values
        "__call",
        NULL
    };

    // Whatever we request, we return a dummy table with all metamethods 
    // set to return another dummy table.
    lua_newtable(L);   
    lua_newtable(L);
    for (size_t i = 0; MetaMethods[i] != NULL; i++)
    {
        lua_pushstring(L, MetaMethods[i]);
        lua_pushcfunction(L, Lua_Dummy);
        lua_settable(L, -3);
    }
    lua_setmetatable(L, -2);
    return 1;
}

int Mod::Lua_Require(lua_State *L)
{
    if (lua_gettop(L) > 0 && lua_isstring(L, -1))
    {
        int idx = lua_upvalueindex(1);
        assert(lua_islightuserdata(L, idx));
        const LuaInfo* info = (const LuaInfo*)lua_touserdata(L, idx);

        Location location(info->f->GetName());
        const char* include = lua_tostring(L, -1);

        ptr<File> f;
        string filename;
        if ((f = Assets::LoadScript(filename = (*info->basepath + include))) == NULL) {
            f = Assets::LoadScript(filename = (string("Library\\") + include));
        }

        if (f == NULL) {
            unknown(location, "script library", filename);
        } else {
            info->mod->CheckScript(Reference(ObjectID(OBJ_SCRIPT, filename), location), *f);
        }
    }
    return 0;
}

static lua_State* LoadScript(File& f)
{
    lua_State* s = lua_open();
    LuaLoadInfo info = {&f};

    if (lua_load(s, LuaFileReader, &info, Utils::GetFilename(f.GetName()).c_str()) != 0)
    {
        // Get and print error
        if (lua_isstring(s, -1))
        {
            // Find the line part of the error (ignore the filename part)
            lua_error(f.GetName(), s);
        }
        else
        {
            error(Location(f.GetName()), "unknown Lua error");
        }
        lua_close(s);
        return NULL;
    }

    // Override the __index event in the global's metatable
    if (lua_getmetatable(s, LUA_GLOBALSINDEX) == 0)
    {
        lua_newtable(s);
    }
    lua_pushstring(s, "__index");
    lua_pushcfunction(s, Lua_Dummy);
    lua_settable(s, -3);
    lua_setmetatable(s, LUA_GLOBALSINDEX);
    return s;
}

void Mod::CheckScript(const Reference& reference, File& f)
{
    lua_State* s = LoadScript(f);
    if (s != NULL)
    {
        // Execute top-level chunk to resolve requires.
        string basepath = Utils::GetBasePath(reference.id.name);
        LuaInfo info = {this, &basepath, &f};

        lua_pushstring(s, "require");
        lua_pushlightuserdata(s, &info);
        lua_pushcclosure(s, Lua_Require, 1);
        lua_settable(s, LUA_GLOBALSINDEX);

        if (lua_pcall(s, 0, 0, 0) != 0)
        {
            lua_error(f.GetName(), s);
        }
        lua_close(s);
    }
}

void Mod::ParseAIScript(const Location &location, const string &filename)
{
    ptr<File> f = LoadAsset(Reference(ObjectID(OBJ_SCRIPT, filename), location));
    if (f != NULL)
    {
        lua_State* s = LoadScript(*f);
        if (s != NULL)
        {
            // Execute top-level chunk to resolve requires.
            string basepath = Utils::GetBasePath(filename);
            LuaInfo info = {this, &basepath, f};

            lua_pushstring(s, "require");
            lua_pushlightuserdata(s, &info);
            lua_pushcclosure(s, Lua_Require, 1);
            lua_settable(s, LUA_GLOBALSINDEX);

            if (lua_pcall(s, 0, 0, 0) != 0)
            {
                lua_error(filename, s);
            }
            else
            {
                // Execute the "Definitions" function to establish the Category
                lua_pushstring(s, "Definitions");
                lua_gettable(s, LUA_GLOBALSINDEX);
                if (lua_pcall(s, 0, 0, 0) != 0)
                {
                    lua_error(filename, s);
                }
                else
                {
                    // Get the category
                    lua_pushstring(s, "Category");
                    lua_gettable(s, LUA_GLOBALSINDEX);
                    if (lua_isstring(s, -1))
                    {
                        // Get the list of goals
                        m_globals.add(Location(filename), lua_tostring(s,-1), "(!)");
                    }
                }
            }
            lua_close(s);
        }
    }
}

void Mod::ParseStory(const Reference& reference, File& f)
{
    try
    {
        struct Event
        {
            Location       location;
            vector<string> prereqs;

            Event(const Location& location) : location(location) {}
        };
        typedef map<string, Event> EventMap;

        EventMap events;

        // Parse the events
        XMLTree tree(f);
        const XMLNode& root = tree.GetRoot();
        for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
        {
            if (p->Equals("Event"))
            {
                const char* cname = p->GetAttribute("Name");
                if (cname == NULL)
                {
                    error(*p, "expected \"Name\" attribute");
                    continue;
                }

                // Parse the event
                Event e(*p);
                for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
                {
                    const char* data = q->GetData();
                    if (q->begin() != q->end())
                    {
                        error(*q, "node should have text; not child nodes");
                    }
                    else if (data != NULL)
                    {
                        if (q->Equals("Prereq"))
                        {
                            e.prereqs = Utils::Split(data, " \r\n\t\f\v,");
                        }
                    }
                }

                // Add the event
                string name(cname);
                transform(name.begin(), name.end(), name.begin(), toupper);

                pair<EventMap::iterator,bool> ins = events.insert(EventMap::value_type(name, e));
                if (!ins.second)
                {
                    duplicate(*p, "event", cname, ins.first->second.location);
                    continue;
                }
            }
        }

        // Validate prerequisites
        for (EventMap::const_iterator p = events.begin(); p != events.end(); ++p)
        {
            for (vector<string>::const_iterator s = p->second.prereqs.begin(); s != p->second.prereqs.end(); ++s)
            {
                if (events.find(Utils::Uppercase(*s)) == events.end())
                {
                    error(p->second.location, "unknown prerequisite \"" + *s + "\"");
                }
            }
        }
    }
    catch (ParseException &e)
    {
        error(reference.location, e.what());
    }
}

void Mod::ParseStoryPlots(const Reference& reference, File& f)
{
    try
    {
        XMLTree tree(f);
        const XMLNode& root = tree.GetRoot();
        for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
        {
            const char* data = p->GetData();
            if (data != NULL)
            {
                if (p->Equals("Active_Plot") || p->Equals("Suspended_Plot"))
                {
                    m_demand->add(Reference(ObjectID(OBJ_STORY, data), reference.location));
                }
                else if (p->Equals("Lua_Script"))
                {
                    m_demand->add(Reference(ObjectID(OBJ_SCRIPT, "Story\\" + string(data)), *p));
                }
            }
        }
    }
    catch (ParseException &e)
    {
        error(reference.location, e.what());
    }
}

void Mod::ParseGoalSet(const Reference& reference, File& f)
{
    try
    {
        XMLTree tree(f);
        const XMLNode& root = tree.GetRoot();
        for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
        {
            string id(Utils::Uppercase(p->GetName()));
            pair<ObjectList::iterator, bool> ins = m_goalSets.insert(make_pair(id, ModObject(*p)));
            if (!ins.second)
            {
                if (ins.first->second.m_location != *p)
                {
                    duplicate(*p, "GoalFunctionSet", p->GetName(), ins.first->second.m_location);
                }
                continue;
            }
            ModObject& object = ins.first->second;
            object.m_name = p->GetName();

            for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
            {
                const char* data = q->GetData();
                if (data != NULL)
                {
                    if (q->Equals("Goal")) {
                        m_demand->add(Reference(ObjectID(OBJ_GOAL, data), *q));
                    } else if (q->Equals("Function")) {
                        m_demand->add(Reference(ObjectID(OBJ_EQUATION, data), *q));
                    }
                }
            }
        }
    }
    catch (ParseException &e)
    {
        error(reference.location, e.what());
    }
}

ModObject* Mod::ParseGoal(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback, bool allow_duplicates)
{
    string id = Utils::Uppercase(node.GetName());
    pair<ObjectList::iterator, bool> r = objects.insert(make_pair(id, ModObject(node)));
    ModObject& object = r.first->second;
    if (!allow_duplicates && !r.second)
    {
        duplicate(node, type, node.GetName(), object.m_location);
        return NULL;
    }

    object.m_name = node.GetName();
    for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
    {
        (this->*callback)(*p, object, tags);
    }
    return &object;
}

ModObject* Mod::ParseTemplate(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback, bool allow_duplicates)
{
    string id = Utils::Uppercase(node.GetName());
    pair<ObjectList::iterator, bool> r = objects.insert(make_pair(id, ModObject(node)));
    ModObject& object = r.first->second;
    if (!allow_duplicates && !r.second)
    {
        duplicate(node, type, node.GetName(), object.m_location);
        return NULL;
    }

    object.m_name = node.GetName();
    for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
    {
        if (p->Equals("Budget"))
        {
            // <Budget> contains a list of equations for AI goal category types
            for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
            {
                object.m_references.add(Reference(ObjectID(OBJ_GOAL_CATEGORY_TYPE, q->GetName()), *q));
                if (q->GetData() != NULL) {
                    object.m_references.add(Reference(ObjectID(OBJ_EQUATION, q->GetData()), *q));
                }
            }
        }
        else if (p->Equals("Turn_On") || p->Equals("Turn_Off"))
        {
            for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
            {
                if (q->Equals("Goals"))
                {
                    for (XMLNode::const_iterator r = q->begin(); r != q->end(); ++r)
                    {
                        if (r->Equals("Category") && r->GetData() != NULL) {
                            object.m_references.add(Reference(ObjectID(OBJ_GOAL_CATEGORY_TYPE, r->GetData()), *q));
                        }
                    }
                }
                else if (q->Equals("Plans"))
                {
                    for (XMLNode::const_iterator r = q->begin(); r != q->end(); ++r)
                    {
                        if (r->GetData() != NULL) {
                            if (r->Equals("Goal_Category")) {
                                object.m_references.add(Reference(ObjectID(OBJ_GOAL_CATEGORY_TYPE, r->GetData()), *q));
                            } else if (r->Equals("Name")) {
                                //object.m_references.add(Reference(ObjectID(OBJ_SCRIPT, string("AI\\") + r->GetData()), *q));
                            }
                        }
                    }
                }
            }
        }
        else
        {
            (this->*callback)(*p, object, tags);
        }
    }
    return &object;
}

ModObject* Mod::ParseEquation(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback, bool allow_duplicates)
{
    string id = Utils::Uppercase(node.GetName());
    pair<ObjectList::iterator, bool> r = objects.insert(make_pair(id, ModObject(node)));
    ModObject& object = r.first->second;
    if (!allow_duplicates && !r.second)
    {
        duplicate(node, type, node.GetName(), object.m_location);
        return NULL;
    }
    object.m_name = node.GetName();

    // Parse equation contents for references to functions or scripts
    const char* data = node.GetData();
    if (data != NULL)
    {
        // First split it up into tokens
        for (const char* s = data; *s != '\0';)
        {
            while (*s != '\0' && !isalnum(*s) && *s != '_') s++;
            if (*s != '\0')
            {
                // Find the end of the token
                const char* end = s;
                while (isalnum(*end) || *end == '_') end++;

                if (_strnicmp(s, "Function_", min(9, end - s)) == 0)
                {
                    object.m_references.add(Reference(ObjectID(OBJ_EQUATION, string(s + 9, end - s - 9)), node));
                }
                else if (_strnicmp(s, "Script_", min(7, end - s)) == 0)
                {
                    object.m_references.add(Reference(ObjectID(OBJ_SCRIPT, "Evaluators\\" + string(s + 7, end - s - 7)), node));
                }
                s = (*end != '\0') ? end + 1 : end;
            }
        }
    }
    return &object;
}

void Mod::ParseAIPlayer(const Location& location, const string& filename)
{
    ptr<File> f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), location));
    if (f != NULL) try
    {
        ModObject object(location);

        XMLTree tree(*f);
        const XMLNode& root = tree.GetRoot();
        for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
        {
            const char* data = p->GetData();
            if (data != NULL)
            {
                if (p->Equals("Name"))
                {
                    object.m_name = data;
                }
                else if (p->Equals("GalacticFreeStoreScript") ||
                         p->Equals("SpaceFreeStoreScript") ||
                         p->Equals("LandFreeStoreScript"))
                {
                    object.m_references.add(Reference(ObjectID(OBJ_SCRIPT, string("FreeStore\\") + data), *p));
                }
                else if (p->Equals("GoalProposalFunctionSets"))
                {
                    vector<string> sets = Utils::Split(data, " \r\n\t\f\v");
                    for (vector<string>::const_iterator s = sets.begin(); s != sets.end(); ++s)
                    {
                        object.m_references.add(Reference(ObjectID(OBJ_GOAL_SET, "AI\\GoalFunctions\\" + *s), *p));
                    }
                }
            }
            else if (p->Equals("Difficulty_Adjustments"))
            {
                for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
                {
                    if ((data = q->GetData()) != NULL &&
                        CheckBuiltin(Reference(ObjectID(OBJ_DIFFICULTY, q->GetName()), *q), Builtins::Difficulties, m_game))
                    {
                        object.m_references.add(Reference(ObjectID(OBJ_DIFFICULTY_ADJUSTMENT, q->GetData()), *q));
                    }
                }
            }
            else if (p->Equals("Templates"))
            {
                for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
                {
                    if ((data = q->GetData()) != NULL &&
                        CheckBuiltin(Reference(ObjectID(OBJ_GAME_MODE, q->GetName()), *q), Builtins::GameModes, m_game))
                    {
                        vector<string> templates = Utils::Split(data, " \r\n\t\f\v");
                        for (vector<string>::const_iterator p = templates.begin(); p != templates.end(); ++p)
                        {
                            object.m_references.add(Reference(ObjectID(OBJ_AI_TEMPLATE, *p), *q));
                        }
                    }
                }
            }
        }

        if (object.m_name.empty())
        {
            error(root, "encountered AI player with no name");
            return;
        }

        string id = Utils::Uppercase(object.m_name);
        pair<ObjectList::iterator, bool> ins = m_aiPlayers.insert(make_pair(id, object));
        if (!ins.second)
        {
            duplicate(root, "AI player", object.m_name, ins.first->second.m_location);
        }
    }
    catch (ParseException &e)
    {
        error(location, e.what());
    }
}

// Tag handler for factions; we need to trap Is_Playable and add a reference
// because it will show up in the faction selection dialog box
void Mod::ParseFaction(const XMLNode& node, ModObject& object, const Tags& tags)
{
    if (node.Equals("Is_Playable") && node.GetData() != NULL)
    {
        if (_stricmp(node.GetData(), "yes") == 0 || _stricmp(node.GetData(), "true")  == 0)
        {
            m_globals.add(Reference(ObjectID(OBJ_FACTION, object.m_name), Location("<root>")));
        }
    }

    // For other nodes take default action
    ParseReferences(node, object, tags);
}

// Tag handler for game objects; some nodes have to be overridden
void Mod::ParseGameObject(const XMLNode& node, ModObject& object, const Tags& tags)
{
    if (node.Equals("Tactical_Buildable_Objects_Campaign") || node.Equals("Tactical_Buildable_Objects_Multiplayer"))
    {
        const char* data = node.GetData();
        if (data != NULL)
        {
            vector<string> ids = Utils::Split(data, ",");
            for (vector<string>::const_iterator p = ids.begin(); p != ids.end(); ++p)
            {
                const string key = Utils::Trim(Utils::Uppercase(*p));
                if (m_factions.find(key) == m_factions.end())
                {
                    // Not a faction? Assume it's a GameObject reference
                    object.m_references.m_references.push_back(Reference(ObjectID(OBJ_GAME_OBJECT, key), node));
                }
            }
        }
        return;
    }
    
    if (node.Equals("Abilities"))
    {
        if (node.GetData() != NULL)
        {
            error(node, "node should have sub-nodes, not data");
            return;
        }
        for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
        {
            CheckBuiltin(Reference(ObjectID(OBJ_ABILITY_CLASS, p->GetName()), *p), Builtins::AbilityClasses, m_game);
            ModObject* ability = ParseObject(*p, "ability", m_abilities, Tags_Ability, &Mod::ParseReferences, true);
            if (ability!= NULL) {
                object.m_references.add(Reference(ObjectID(OBJ_ABILITY, ability->m_name), *p));
            }
        }
        return;
    }

    if (node.Equals("Unit_Abilities_Data"))
    {
        if (node.GetData() != NULL)
        {
            error(node, "node should have sub-nodes, not data");
            return;
        }
        for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
        {
            ModObject ability(*p);
            for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
            {
                if (q->GetData() != NULL) {
                    if (q->Equals("GUI_Activated_Ability_Name")) {
                        ability.m_name = q->GetData();
                    } else if (q->Equals("Type")) {
                        CheckBuiltin(Reference(ObjectID(OBJ_ABILITY_TYPE, q->GetData()), *q), Builtins::AbilityTypes, m_game);
                    } else if (q->Equals("Alternate_Description_Text")) {
                        object.m_references.add(*q, q->GetData(), "(s)");
                    } else if (q->Equals("SFXEvent_GUI_Unit_Ability_Activated")) {
                        object.m_references.add(*q, q->GetData(), "X");
                    }
                }
            }
            
            if (!ability.m_name.empty())
            {
                // Ignore duplicates
                m_abilities.insert(make_pair(Utils::Uppercase(ability.m_name), ability));
            }
        }
        return;
    }

    // For other nodes take default action
    ParseReferences(node, object, tags);
}

ModObject* Mod::ParseObject(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback, bool allow_duplicates)
{
    // Check that we have a name
    const char* name = node.GetAttribute("Name");
    if (name == NULL)
    {
        error(node, "encountered " + string(type) + " with no name");
        return NULL;
    }

    string id = Utils::Uppercase(name);
    pair<ObjectList::iterator, bool> r = objects.insert(make_pair(id, ModObject(node)));
    ModObject& object = r.first->second;
    if (!allow_duplicates && !r.second)
    {
        duplicate(node, type, name, object.m_location);
        return NULL;
    }

    object.m_name = name;
    for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
    {
        (this->*callback)(*p, object, tags);
    }
    return &object;
}

void Mod::ParseObjects(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const FileCallback& callback, const ObjectCallback& ocallback)
{
    for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
    {
        (this->*callback)(*p, type, objects, tags, ocallback, false);
    }
}

void Mod::ParseFile(const Location& loc, const char* filename, const char* type, ObjectList& objects, const Tags& tags, const FileCallback& callback, const ObjectCallback& ocallback)
{
    ptr<File> f;
    if ((f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), loc))) != NULL)
    {
        try
        {
            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            ParseObjects(root, type, objects, tags, callback, ocallback);
        }
        catch (ParseException& e)
        {
            error(loc, e.what());
        }
    }
}

void Mod::ParseIndexFile(const Location& loc, const char* filename, const char* type, ObjectList& objects, const Tags& tags, const FileCallback& callback, const ObjectCallback& ocallback)
{
    ptr<File> f;
    if ((f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), loc))) != NULL)
    {
        try
        {
            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
            {
                const char* data = p->GetData();
                if (p->Equals("File") && data != NULL)
                {
                    ParseFile(*p, data, type, objects, tags, callback, ocallback);
                }
            }
        }
        catch (ParseException& e)
        {
            cerr << e.what() << endl;
        }
    }
}

void Mod::ParseRadarMap(const Location& loc, const char* filename)
{
    ptr<File> f;
    if ((f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), loc))) != NULL)
    {
        try
        {
            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
            {
                const char* data = p->GetData();
                if (p->Equals("RadarMapEvents"))
                {
                    for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
                    {
                        ParseObject(*q, "radar map event", m_radarMapEvents, Tags_RadarMapEvent);
                    }
                }
                else if (p->Equals("RadarMapSettings"))
                {
                    for (XMLNode::const_iterator q = p->begin(); q != p->end(); ++q)
                    {
                        m_globals.add(*q, Tags_RadarMapSettings);
                    }
                }
            }
        }
        catch (ParseException& e)
        {
            cerr << e.what() << endl;
        }
    }
}

void Mod::ParseTradeRouteLine(const XMLNode& node, ModObject& object, const Tags& tags)
{
    if (node.Equals("Settings_For_Faction"))
    {
        for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
        {
            ParseReferences(*p, object, tags);
        }
    }
}

void Mod::ParseWeatherAudio(const Location& loc, const char* filename)
{
    ptr<File> f;
    if ((f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), loc))) != NULL)
    {
        try
        {
            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
            {
                m_globals.add(*p, Tags_WeatherAudio);
            }
        }
        catch (ParseException& e)
        {
            cerr << e.what() << endl;
        }
    }
}

void Mod::ParseWeatherScenarioPhase(const XMLNode& node, ModObject& object, const Tags& tags)
{
    if (node.Equals("Phase"))
    {
        const char* name = node.GetAttribute("Name");
        if (name == NULL) {
            error(node, "expected \"Name\" attribute with weather type");
        } else {
            CheckBuiltin(Reference(ObjectID(OBJ_WEATHER_TYPE, name), node), Builtins::WeatherTypes, m_game);
        }

        for (XMLNode::const_iterator p = node.begin(); p != node.end(); ++p)
        {
            ParseReferences(*p, object, tags);
        }
    }
}

void Mod::ParseWeatherModifierModifier(const XMLNode& node, ModObject& object, const Tags& tags)
{
    const TagInfo* ti = tags.find(node.GetName());
    if (ti != NULL)
    {
        ParseReferences(node, object, tags);
    }
    else if (node.Equals("Sight_Range_Modifier") ||
             node.Equals("Speed_Modifier") ||
             node.Equals("Weapon_Accuracy_Modifier"))
    {
        const char* name = node.GetAttribute("name");
        if (name == NULL)
        {
            error(node, "expected \"Name\" attribute");
        }
    }
}

ModObject* Mod::ParseWeatherScenario(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback, bool allow_duplicates)
{
    ModObject* object = ParseObject(node, type, objects, tags, callback, allow_duplicates);
    if (object != NULL)
    {
        const char* emitter = node.GetAttribute("emitter_name");
        if (emitter != NULL)
        {
            object->m_references.add(Reference(ObjectID(OBJ_PARTICLE, emitter), node));
        }
    }
    return object;
}

ModObject* Mod::ParseWeatherModifier(const XMLNode& node, const char* type, ObjectList& objects, const Tags& tags, const ObjectCallback& callback, bool allow_duplicates)
{
    ModObject* object = ParseObject(node, type, objects, tags, callback, allow_duplicates);
    if (object != NULL)
    {
        const char* name = node.GetAttribute("Name");
        if (name != NULL)
        {
            CheckBuiltin(Reference(ObjectID(OBJ_WEATHER_TYPE, name), node), Builtins::WeatherTypes, m_game);
        }
    }
    return object;
}

void Mod::ParseAnimationSFXMaps(const Location& location, const char* filename)
{
    ptr<File> f = LoadAsset(Reference(ObjectID(OBJ_FILE, filename), location));
    if (f != NULL && f->GetSize() > 0)
    {
        vector<char> buffer(f->GetSize());
        if (f->Read(&buffer[0], buffer.size()) != buffer.size()) {
            throw runtime_error("Unable to read file: " + f->GetName());
        }

        Location location(filename);

        vector<string> lines = Utils::Split(string(&buffer[0], buffer.size()), "\r\n");
        for (size_t i = 0; i < lines.size(); i++)
        {
            location.line = i + 1;

            vector<string> tokens = Utils::Split(lines[i], " \t\v\f");
            if (tokens.size() >= 4)
            {
                // Check the animation
                LoadAsset(Reference(ObjectID(OBJ_ANIMATION, tokens[1]), location));

                if (_stricmp(tokens[0].c_str(), "SOUND") == 0) {
                    m_globals.add(Reference(ObjectID(OBJ_SFXEVENT, tokens[3]), location));
                } else if (_stricmp(tokens[0].c_str(), "SURFACE") == 0) {
                    m_globals.add(Reference(ObjectID(OBJ_SURFACE_FX, tokens[3]), location));
                } else {
                    error(location, "unknown animation action (should be SOUND or SURFACE)");
                }
            }
        }
    }
}

void Mod::ParseGameConstants(const Location& location, const char* filename)
{
    ptr<File> f;
    if ((f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), location))) != NULL)
    {
        try
        {
            bool haveDamageTypes = false;
            bool haveArmorTypes  = false;

            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
            {
                const char* data = p->GetData();
                if (p->Equals("Damage_Types"))
                {
                    if (haveDamageTypes) {
                        error(*p, "duplicate \"Damage_Types\" tag");
                    } else if (p->begin() != p->end()) {
                        error(*p, "expected data in node");
                    } else if (data != NULL) {
                        vector<string> types = Utils::Split(data, WHITESPACE ",");
                        for (vector<string>::iterator t = types.begin(); t != types.end(); ++t)
                        {
                            transform(t->begin(), t->end(), t->begin(), toupper);
                            if (!m_damageTypes.insert(Utils::Trim(*t)).second) {
                                error(*p, "duplicate damage type");
                            }
                        }
                    }
                    haveDamageTypes = true;
                }
                else if (p->Equals("Armor_Types"))
                {
                    if (haveArmorTypes) {
                        error(*p, "duplicate \"Armor_Types\" tag");
                    } else if (p->begin() != p->end()) {
                        error(*p, "expected data in node");
                    } else if (data != NULL) {
                        vector<string> types = Utils::Split(data, WHITESPACE ",");
                        for (vector<string>::iterator t = types.begin(); t != types.end(); ++t)
                        {
                            transform(t->begin(), t->end(), t->begin(), toupper);
                            if (!m_armorTypes.insert(Utils::Trim(*t)).second) {
                                error(*p, "duplicate armor type");
                            }
                        }
                    }
                    haveArmorTypes = true;
                }
                else
                {
                    m_globals.add(*p, Tags_GameConstants);
                }
            }
        }
        catch (ParseException& e)
        {
            cerr << e.what() << endl;
        }
    }
}

void Mod::ParseAudio(const Location& location, const char* filename)
{
    ptr<File> f;
    if ((f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), location))) != NULL)
    {
        try
        {
            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
            {
                m_globals.add(*p, Tags_Audio);
            }
        }
        catch (ParseException& e)
        {
            cerr << e.what() << endl;
        }
    }
}

void Mod::ParseEnumeration(const Location& location, const char* filename, const string& type, DefinitionList& definitions)
{
    ptr<File> f;
    if ((f = LoadAsset(Reference(ObjectID(OBJ_XML, filename), location))) != NULL)
    {
        try
        {
            bool haveDamageTypes = false;
            bool haveArmorTypes  = false;

            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
            {
                const char* data = p->GetData();
                if (p->begin() != p->end()) {
                    error(*p, "expected data in node");
                } else if (data != NULL) {
                    string value(data);
                    if (!IsObjOfType(value, OBJ_INTEGER)) {
                        error(*p, "expected integer value");
                    } else if (!definitions.insert(Utils::Uppercase(p->GetName())).second) {
                        error(*p, "duplicate " + type);
                    }
                }
            }
        }
        catch (ParseException& e)
        {
            cerr << e.what() << endl;
        }
    }
}

void Mod::LoadReferenceObjects(ChecksumMap& checksums)
{
    try {
        ptr<File> f;
        if ((f = Assets::LoadXML("GameObjectFiles.xml")) != NULL)
        {
            XMLTree xml(*f);
            const XMLNode& root = xml.GetRoot();
            for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
            {
                if (p->Equals("File") && p->GetData() != NULL && (f = Assets::LoadXML(Utils::Trim(p->GetData()))) != NULL) try
                {
                    XMLTree xml(*f);
                    const XMLNode& root = xml.GetRoot();
                    for (XMLNode::const_iterator p = root.begin(); p != root.end(); ++p)
                    {
                        const char* cname = p->GetAttribute("Name");
                        if (cname != NULL) {
                            string name(cname);
                            transform(name.begin(), name.end(), name.begin(), toupper);
                            checksums.insert(make_pair(Utils::CRC32(name.c_str(), name.length()), cname));
                        }
                    }
                } catch (ParseException&) {
                }
            }
        }
    } catch (ParseException&) {
    }
}

void Mod::Load()
{
    Location root(""); // We need a "dummy" location for the root

    ParseAnimationSFXMaps(root, "Data\\XML\\AnimationSFXMaps.txt");
    ParseGameConstants(root, "GameConstants.xml");
    ParseEnumeration(root, "Enum\\GameObjectCategoryType.xml",   "game object category",    m_categories);
    ParseEnumeration(root, "Enum\\GameObjectPropertiesType.xml", "game object properties",  m_gameObjectProperties);
    ParseEnumeration(root, "Enum\\SurfaceFXTriggerType.xml",     "surface FX trigger type", m_surfaceFXTriggerTypes);
    ParseEnumeration(root, "Enum\\MovementClassType.xml",        "movement class type",     m_movementClasses);
    ParseEnumeration(root, "Enum\\AIGoalCategoryType.xml",       "AI goal category type",   m_aiGoalCategoryTypes);
    
    // Factions must occur before GameObjects !!!
    ParseIndexFile(root, "FactionFiles.xml",    "faction",     m_factions,    Tags_Faction, &Mod::ParseObject, &Mod::ParseFaction);
    ParseIndexFile(root, "GameObjectFiles.xml", "game object", m_gameObjects, Tags_GameObject, &Mod::ParseObject, &Mod::ParseGameObject);

    // Create checksum lookup for GameObjects
    for (ObjectList::const_iterator p = m_gameObjects.begin(); p != m_gameObjects.end(); ++p)
    {
        m_checksums.insert(make_pair(Utils::CRC32(p->first.c_str(), p->first.length()), p->first));
    }

    // Enumerate and parse Maps
    // Must occurs after GameObjects (and checksums) !!!
    ptr<Assets::IEnumerator> enumerator = Assets::Enumerate("Data\\Art\\Maps\\*.ted");
    if (enumerator != NULL) do {
        ptr<File> f = LoadAsset(Reference(ObjectID(OBJ_MAP, enumerator->GetFileName().substr(14)), Location("<root>")));
        if (f != NULL) try
        {
            string      map_name = Utils::GetFilename(f->GetName());
            Assets::Map map(*f, true);
            if (map.GetProperties().m_numPlayers > 1) {
                // It's a multiplayer map, add it to the global references
                m_globals.add(Reference(ObjectID(OBJ_MAP, map_name), root));
            }
        }
        catch (BadFileException&)
        {
            error(root, "Bad map file: " + f->GetName());
        }
    } while (enumerator->Next());

    ParseIndexFile(root, "TradeRouteFiles.xml",          "traderoute",             m_tradeRoutes,         Tags_TradeRoute);
	ParseIndexFile(root, "HardPointDataFiles.xml",       "hard point",             m_hardpoints,          Tags_Hardpoint);
    ParseIndexFile(root, "SFXEventFiles.xml",            "sound event",            m_sfxEvents,           Tags_SFXEvent);
	ParseIndexFile(root, "TargetingPrioritySetFiles.xml","targeting priority set", m_targetingPriorities, Tags_TargetingPriority);
	ParseIndexFile(root, "CommandBarComponentFiles.xml", "commandbar component",   m_commandbarComponents,Tags_CommandbarComponent);
    ParseIndexFile(root, "MousePointerFiles.xml",        "mouse pointer",          m_mousePointers,       Tags_MousePointer);
    ParseIndexFile(root, "CampaignFiles.xml",            "campaign",               m_campaigns,           Tags_Campaign, &Mod::ParseObject, &Mod::ParseCampaign);
    ParseFile     (root, "MusicEvents.xml",              "music event",            m_musicEvents,         Tags_MusicEvent);
    ParseFile     (root, "SpeechEvents.xml",             "speech event",           m_speechEvents,        Tags_SpeechEvent);
    ParseFile     (root, "ShadowBlobMaterials.xml",      "shadow blob",            m_shadowBlobs,         Tags_ShadowBlob);
    ParseFile     (root, "TerrainDecalFX.xml",           "terrain decal",          m_terrainDecals,       Tags_TerrainDecal);
    ParseFile     (root, "DynamicTrackFX.xml",           "dynamic track",          m_dynamicTracks,       Tags_DynamicTrack);
    ParseFile     (root, "SurfaceFX.xml",                "surface FX",             m_surfaceFXs,          Tags_SurfaceFX, &Mod::ParseObject, &Mod::ParseSurfaceFX);
    ParseFile     (root, "Movies.xml",                   "movie",                  m_movies,              Tags_ShadowBlob);
    ParseFile     (root, "StarWars3DTextCrawl.xml",      "text crawl",             m_textCrawls,          Tags_TextCrawl);
    ParseFile     (root, "LightningEffectTypes.xml",     "lightning effect",       m_lightningEffects,    Tags_LightningEffect);
    ParseFile     (root, "HeroClash.xml",                "hero clash",             m_heroClashes,         Tags_HeroClash);
	ParseFile     (root, "TacticalCameras.xml",          "tactical camera",        m_tacticalCameras,     Tags_TacticalCamera);
    ParseFile     (root, "TradeRouteLines.xml",          "traderoute line",        m_tradeRouteLines,     Tags_TradeRouteLine, &Mod::ParseObject, &Mod::ParseTradeRouteLine);
	ParseFile     (root, "DifficultyAdjustments.xml",    "difficulty adjustment",  m_difficulties,        Tags_Difficulty);
    ParseFile     (root, "WeatherScenarios.xml",         "weather scenario",       m_weatherScenarios,    Tags_WeatherScenario, &Mod::ParseWeatherScenario, &Mod::ParseWeatherScenarioPhase);
    ParseFile     (root, "WeatherModifiers.xml",         "weather modifier",       m_weatherModifiers,    Tags_WeatherModifier, &Mod::ParseWeatherModifier, &Mod::ParseWeatherModifierModifier);
    ParseFile     (root, "LensFlares.xml",               "lens flare",             m_lensFlares,          Tags_LensFlare);
    if (m_game == GID_EAW_FOC)
    {
        ParseFile (root, "BlackMarketItems.xml",         "black market item",      m_blackMarketItems,    Tags_BlackMarketItem);
    }
    ParseAudio    (root, "Audio.xml");
    ParseRadarMap (root, "RadarMap.xml");
    ParseWeatherAudio(root, "WeatherAudio.xml");

    // Enumerate and parse AI players
    ptr<Assets::IEnumerator> enumator = Assets::Enumerate("Data\\XML\\AI\\Players\\*.xml");
    if (enumator != NULL) do {
        ParseAIPlayer(root, enumator->GetFileName().substr(9));
    } while (enumator->Next());

    // Enumerate and parse Goals
    enumator = Assets::Enumerate("Data\\XML\\AI\\Goals\\*.xml");
    if (enumator != NULL) do {
        ParseFile(root, enumator->GetFileName().substr(9).c_str(), "goal", m_goals, Tags_Goal, &Mod::ParseGoal);
    } while (enumator->Next());

    // Enumerate and parse Templates
    enumator = Assets::Enumerate("Data\\XML\\AI\\Templates\\*.xml");
    if (enumator != NULL) do {
        ParseFile(root, enumator->GetFileName().substr(9).c_str(), "AI template", m_aiTemplates, Tags_Template, &Mod::ParseTemplate);
    } while (enumator->Next());

    // Enumerate and parse PerceptualEquations
    enumator = Assets::Enumerate("Data\\XML\\AI\\PerceptualEquations\\*.xml");
    if (enumator != NULL) do {
        ParseFile(root, enumator->GetFileName().substr(9).c_str(), "perceptual equation", m_equations, Tags_Goal, &Mod::ParseEquation);
    } while (enumator->Next());

    // Enumerate and parse AI scripts
    enumator = Assets::Enumerate("Data\\Scripts\\AI\\*.lua");
    if (enumator != NULL) do {
        ParseAIScript(root, enumator->GetFileName().substr(13));
    } while (enumator->Next());

    // These essentials should be defined
    m_globals.add(Reference(ObjectID(OBJ_MUSIC_EVENT, "Main_Menu_Music_Event"), root));
    m_globals.add(Reference(ObjectID(OBJ_TEXTURE, "d_master_decal.tga"), root));
    m_globals.add(Reference(ObjectID(OBJ_TACTICAL_CAMERA, "Land_Mode"),  root));
    m_globals.add(Reference(ObjectID(OBJ_TACTICAL_CAMERA, "Space_Mode"), root));
    m_globals.add(Reference(ObjectID(OBJ_TACTICAL_CAMERA, "Unlocked"),   root));
    m_globals.add(Builtins::MousePointers, OBJ_MOUSE_POINTER,    root, m_game);
    m_globals.add(Builtins::WeatherTypes,  OBJ_WEATHER_MODIFIER, root, m_game);

    // All loaded campaigns are also used
    for (ObjectList::const_iterator p = m_campaigns.begin(); p != m_campaigns.end(); ++p)
    {
        m_globals.add(Reference(ObjectID(OBJ_CAMPAIGN, p->second.m_name), root));
    }

    // All loaded command bar components are also used
    for (ObjectList::const_iterator p = m_commandbarComponents.begin(); p != m_commandbarComponents.end(); ++p)
    {
        m_globals.add(Reference(ObjectID(OBJ_COMMANDBAR_COMPONENT, p->second.m_name), root));
    }

    // All loaded black market items are also used
    for (ObjectList::const_iterator p = m_blackMarketItems.begin(); p != m_blackMarketItems.end(); ++p)
    {
        m_globals.add(Reference(ObjectID(OBJ_BLACKMARKETITEM, p->second.m_name), root));
    }
}

void Mod::Validate()
{
    // Validate the references. We do this by keeping:
    // * a queue of reference lists to check.
    // * a list of all validated objects IDs.
    // We validate from the root outwards, to avoid attempting to validate
    // objects which are never used. Thus, validating a reference can add
    // additional references to validate. To recognize recursive references,
    // we also keep the list of already-validated object IDs.
    queue<const ReferenceList*> references;
    set<Reference>              checked;

    // Initialize the list with the root references
    references.push(&m_globals);

    // Initialize the first on-demand reference list
    m_demands.push_back(ReferenceList());
    m_demand = &m_demands.back();

    // Commence validation
    while (!references.empty())
    {
        const ReferenceList& refs = *references.front();
        for (list<Reference>::const_iterator p = refs.m_references.begin(); p != refs.m_references.end(); ++p)
        {
            Reference ref(*p);
            transform(ref.id.name.begin(), ref.id.name.end(), ref.id.name.begin(), toupper);

            if (checked.insert(ref).second)
            {
                // We haven't checked this one before
                const BuiltinInfo* builtin     = NULL;
                const ObjectList*  object_list = NULL;
                OnDemandCheck      on_demand   = NULL;

                bool success = false;
                switch (p->id.type)
                {
                    // Trivial references
                    case OBJ_FLOAT:
                    case OBJ_INTEGER:
                    case OBJ_BOOLEAN:           success = true; break;

                    // Hardcoded references
                    case OBJ_LANGUAGE:          builtin = &Builtins::Languages; break;
                    case OBJ_HARDPOINT_TYPE:    builtin = &Builtins::HardpointTypes; break;
                    case OBJ_STORY_TRIGGER:     builtin = &Builtins::StoryTriggers; break;
                    case OBJ_STORY_REWARD:      builtin = &Builtins::StoryRewards; break;
                    case OBJ_ABILITY_TYPE:      builtin = &Builtins::AbilityTypes; break;
                    case OBJ_ACTIVATION_STYLE:  builtin = &Builtins::ActivationStyles; break;
                    case OBJ_TERRAIN_TYPE:      builtin = &Builtins::TerrainTypes; break;
                    case OBJ_RENDER_MODE:       builtin = &Builtins::RenderModes; break;
                    case OBJ_WEATHER_TYPE:      builtin = &Builtins::WeatherTypes; break;
                    case OBJ_VICTORY_CONDITION: builtin = &Builtins::VictoryConditions; break;
                    case OBJ_GAME_MODE:         builtin = &Builtins::GameModes; break;
                    case OBJ_DIFFICULTY:        builtin = &Builtins::Difficulties; break;
                    case OBJ_AI_GOAL_FLAG:      builtin = &Builtins::AIGoalApplicationFlags; break;
                   
                    // Enumeration references
                    case OBJ_DAMAGE_TYPE:       success = CheckDefinition(*p, m_damageTypes); break;
                    case OBJ_ARMOR_TYPE:        success = CheckDefinition(*p, m_armorTypes); break;
                    case OBJ_CATEGORY:          success = CheckDefinition(*p, m_categories); break;
                    case OBJ_MOVEMENT_CLASS:    success = CheckDefinition(*p, m_movementClasses); break;
                    case OBJ_OBJECT_PROPERTY:   success = CheckDefinition(*p, m_gameObjectProperties); break;
                    case OBJ_GOAL_CATEGORY_TYPE:success = CheckDefinition(*p, m_aiGoalCategoryTypes); break;

                    // Asset references
                    case OBJ_FILE:
                    case OBJ_TEXTURE:
                    case OBJ_SFX:
                    case OBJ_MUSIC:
                    case OBJ_SPEECH:
                    case OBJ_SHADER:
                    case OBJ_ANIMATION:
                    case OBJ_CINEMATIC:         success = (LoadAsset(*p, false) != NULL); break;
                    case OBJ_MAP:               on_demand = &Mod::CheckMap; break;
                    case OBJ_MODEL_OR_PARTICLE: on_demand = &Mod::CheckModelOrParticle; break;
                    case OBJ_MODEL:             on_demand = &Mod::CheckModel; break;
                    case OBJ_PARTICLE:          object_list = &m_lensFlares;
                                                on_demand = &Mod::CheckParticle; break;
                    case OBJ_SCRIPT:            on_demand = &Mod::CheckScript; break;
                    case OBJ_MTD_TEXTURE:       success = CheckMTD(*p, m_mtd); break;
                    case OBJ_STRING:            success = CheckString(*p, m_strings); break;

                    // Object references
                    case OBJ_GAME_OBJECT:       object_list = &m_gameObjects; break;
                    case OBJ_FACTION:           object_list = &m_factions; break;
                    case OBJ_RADAR_MAP_EVENT:   object_list = &m_radarMapEvents; break;
                    case OBJ_ABILITY:           object_list = &m_abilities; break;
                    case OBJ_SFXEVENT:          object_list = &m_sfxEvents; break;
                    case OBJ_LIGHTNING_EFFECT:  object_list = &m_lightningEffects; break;
                    case OBJ_SHADOW_BLOB:       object_list = &m_shadowBlobs; break;
                    case OBJ_MUSIC_EVENT:       object_list = &m_musicEvents; break;
                    case OBJ_SPEECH_EVENT:      object_list = &m_speechEvents; break;
                    case OBJ_MOVIE:             object_list = &m_movies; break;
                    case OBJ_TEXT_CRAWL:        object_list = &m_textCrawls; break;
                    case OBJ_TERRAIN_DECAL:     object_list = &m_terrainDecals; break;
                    case OBJ_SURFACE_FX:        object_list = &m_surfaceFXs; break;
                    case OBJ_DYNAMIC_TRACK:     object_list = &m_dynamicTracks; break;
                    case OBJ_TACTICAL_CAMERA:   object_list = &m_tacticalCameras; break;
                    case OBJ_TRADE_ROUTE:       object_list = &m_tradeRoutes; break;
                    case OBJ_TRADE_ROUTE_LINE:  object_list = &m_tradeRouteLines; break;
                    case OBJ_HARDPOINT:         object_list = &m_hardpoints; break;
                    case OBJ_DIFFICULTY_ADJUSTMENT: object_list = &m_difficulties; break;
                    case OBJ_TARGETING_PRIORITY:object_list = &m_targetingPriorities; break;
                    case OBJ_WEATHER_SCENARIO:  object_list = &m_weatherScenarios; break;
                    case OBJ_WEATHER_MODIFIER:  object_list = &m_weatherModifiers; break;
                    case OBJ_MOUSE_POINTER:     object_list = &m_mousePointers; break;
                    case OBJ_CAMPAIGN:          object_list = &m_campaigns; break;
                    case OBJ_AI_PLAYER:         object_list = &m_aiPlayers; break;
                    case OBJ_AI_TEMPLATE:       object_list = &m_aiTemplates; break;
                    case OBJ_STORY_PLOT:        on_demand = &Mod::ParseStoryPlots; break;
                    case OBJ_STORY:             on_demand = &Mod::ParseStory; break;
                    case OBJ_GOAL_SET:          on_demand = &Mod::ParseGoalSet; break;
                    case OBJ_GOAL:              object_list = &m_goals; break;
                    case OBJ_EQUATION:          object_list = &m_equations; break;
                    case OBJ_BLACKMARKETITEM:   object_list = &m_blackMarketItems; break;
                    case OBJ_COMMANDBAR_COMPONENT: object_list = &m_commandbarComponents; break;

                    case OBJ_GAME_OBJECT_CRC:
                    {
                        unsigned long crc;
                        stringstream  ss;
                        ss << p->id.name;
                        ss >> crc;
                        ChecksumMap::const_iterator c = m_checksums.find(crc);
                        if (c != m_checksums.end()) {
                            // The find will succeed
                            references.push(&m_gameObjects.find(c->second)->second.m_references);
                        } else {
                            unknown_crc(p->location, crc, m_reference);
                        }
                        success = true;
                    }
 
                    default:
                        assert(0);
                        break;
                }

                if (!success && object_list != NULL)
                {
                    const ModObject* obj = FindObject(*p, *object_list);
                    if (obj != NULL) {
                        references.push(&obj->m_references);
                        success = true;
                    }
                }

                if (!success && builtin != NULL)
                {
                    success = CheckBuiltin(*p, *builtin, m_game, false);
                }

                if (!success && on_demand != NULL) {
                    ptr<File> f = LoadAsset(*p, false);
                    if (f != NULL)
                    {
                        (this->*on_demand)(*p, *f);
                        success = true;
                    }
                }

                if (!success) {
                    unknown(p->location, GetObjTypeName(p->id.type), p->id.name);
                }
            }
        }
        references.pop();

        if (references.empty())
        {
            // We've depleted all references, now append the on-demand references so far and continue
            if (!m_demand->m_references.empty())
            {
                references.push(m_demand);
                m_demands.push_back(ReferenceList());
                m_demand = &m_demands.back();
            }
        }
    }
}

Mod::Mod(GameID game, const ChecksumMap& reference)
    : m_demand(NULL), m_game(game), m_reference(reference)
{
#ifndef NDEBUG
    // In debug mode, validate that the hardcoded arrays are sorted.
    ValidateTags();
    ValidateBuiltins();
#endif

    ptr<File> f = Assets::LoadTexture("MT_CommandBar.mtd");
    if (f == NULL) {
        cerr << "error: unable to load \"MT_CommandBar.mtd\"" << endl;
    } else {
        m_mtd.reset(new MegaTextureDirectory(f));
    }

    f = NULL;
    for (size_t i = 0; f == NULL && i < Builtins::Languages.count; i++)
    {
        string filename = "MasterTextFile_" + Utils::Uppercase(Builtins::Languages.list[i].name) + ".dat";
        f = Assets::LoadFile("Data\\Text\\" + filename);
    }
    
    if (f == NULL) {
        cerr << "error: unable to load a MasterTextFile for any language" << endl;
    } else {
        m_strings.reset(new StringList(f));
    }

    Load();
    Validate();
}