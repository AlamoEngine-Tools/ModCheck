#include "Tags.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
using namespace std;

const char* GetObjTypeName(ObjType type) {
    switch (type)
    {
    case OBJ_FLOAT:                return "float";
    case OBJ_INTEGER:              return "integer";
    case OBJ_BOOLEAN:              return "boolean";

    case OBJ_LANGUAGE:             return "language";
    case OBJ_HARDPOINT_TYPE:       return "hardpoint type";
    case OBJ_STORY_TRIGGER:        return "story trigger type";
    case OBJ_STORY_REWARD:         return "story reward type";
    case OBJ_ABILITY_TYPE:         return "ability type";
    case OBJ_ACTIVATION_STYLE:     return "activation style";
    case OBJ_ABILITY_CLASS:        return "ability class";
    case OBJ_TERRAIN_TYPE:         return "terrain type";
    case OBJ_RENDER_MODE:          return "render mode";
    case OBJ_WEATHER_TYPE:         return "weather type";
    case OBJ_VICTORY_CONDITION:    return "victory condition";
    case OBJ_GAME_MODE:            return "game mode";
    case OBJ_DIFFICULTY:           return "difficulty";
    case OBJ_AI_GOAL_FLAG:         return "AI goal application flag";

    case OBJ_DAMAGE_TYPE:          return "damage type";
    case OBJ_ARMOR_TYPE:           return "armor type";
    case OBJ_CATEGORY:             return "game object category";
    case OBJ_MOVEMENT_CLASS:       return "movement class";
    case OBJ_OBJECT_PROPERTY:      return "game object property";
    case OBJ_GOAL_CATEGORY_TYPE:   return "AI goal category type";

    case OBJ_FILE:                 return "file";
    case OBJ_TEXTURE:              return "texture";
    case OBJ_MODEL:                return "model";
    case OBJ_ANIMATION:            return "animation";
    case OBJ_MODEL_OR_PARTICLE:    return "model or particle";
    case OBJ_PARTICLE:             return "particle";
    case OBJ_MAP:                  return "map";
    case OBJ_SFX:                  return "sound file";
    case OBJ_MUSIC:                return "music file";
    case OBJ_SPEECH:               return "speech file";
    case OBJ_MTD_TEXTURE:          return "MTD texture";
    case OBJ_STRING:               return "string";
    case OBJ_CINEMATIC:            return "cinematic";
    case OBJ_SCRIPT:               return "script";
    case OBJ_XML:                  return "XML file";
    case OBJ_SHADER:               return "shader";

    case OBJ_GAME_OBJECT:          return "game object";
    case OBJ_GAME_OBJECT_CRC:      return "game object checksum";
    case OBJ_FACTION:              return "faction";
    case OBJ_RADAR_MAP_EVENT:      return "radar map event";
    case OBJ_ABILITY:              return "ability";
    case OBJ_SFXEVENT:             return "SFX event";
    case OBJ_LIGHTNING_EFFECT:     return "lightning effect";
    case OBJ_SHADOW_BLOB:          return "shadow blob";
    case OBJ_MUSIC_EVENT:          return "music event";
    case OBJ_SPEECH_EVENT:         return "speech event";
    case OBJ_MOVIE:                return "movie";
    case OBJ_TEXT_CRAWL:           return "text crawl";
    case OBJ_TERRAIN_DECAL:        return "terrain decal";
    case OBJ_SURFACE_FX:           return "surface FX";
    case OBJ_DYNAMIC_TRACK:        return "dynamic track";
    case OBJ_TACTICAL_CAMERA:      return "tactical camera";
    case OBJ_TRADE_ROUTE:          return "trade route";
    case OBJ_TRADE_ROUTE_LINE:     return "trade route line";
    case OBJ_HARDPOINT:            return "hardpoint";
    case OBJ_DIFFICULTY_ADJUSTMENT:return "difficulty adjustment";
    case OBJ_TARGETING_PRIORITY:   return "targeting priority set";
    case OBJ_COMMANDBAR_COMPONENT: return "commandbar component";
    case OBJ_WEATHER_SCENARIO:     return "weather scenario";
    case OBJ_WEATHER_MODIFIER:     return "weather modifier";
    case OBJ_MOUSE_POINTER:        return "mouse pointer";
    case OBJ_CAMPAIGN:             return "campaign";
    case OBJ_STORY:                return "story file";
    case OBJ_STORY_PLOT:           return "story plots file";
    case OBJ_AI_PLAYER:            return "AI player";
    case OBJ_AI_TEMPLATE:          return "AI template";
    case OBJ_GOAL_SET:             return "goal function set";
    case OBJ_GOAL:                 return "goal";
    case OBJ_EQUATION:             return "perceptual equation";
    case OBJ_BLACKMARKETITEM:      return "black market item";
    }
    assert(0);
    return "";
}

bool IsObjOfType(const string& value, ObjType type) {
    switch (type)
    {
    case OBJ_FLOAT:
    {
        char *endptr;
        strtod(value.c_str(), &endptr);
        return (*endptr == '\0');
    }

    case OBJ_INTEGER:
    {
        char* endptr;
        strtol(value.c_str(), &endptr, 0);
        return (*endptr == '\0');
    }

    case OBJ_BOOLEAN:
        return _stricmp(value.c_str(), "yes")   == 0 ||
               _stricmp(value.c_str(), "true")  == 0 ||
               _stricmp(value.c_str(), "no")    == 0 ||
               _stricmp(value.c_str(), "false") == 0;
    
    }
    // Everything else is a reference; we can't check that here.
    return true;
}

//
// We terminate each tag list with a special marker so:
// * the array isn't possibly empty
// * we can validate the list at startup in debug mode.
//
static const char* MAGIC_STRING = "__\x42Magic__";
#define BEGIN_TAGS(name, num)                        \
    extern const TagInfo List_##name[num+1];         \
    const Tags Tags_##name = {List_##name, (num+1)}; \
    static const TagInfo List_##name[num+1] = {
#define END_TAGS {MAGIC_STRING} }

BEGIN_TAGS(None, 0)
END_TAGS;

BEGIN_TAGS(TerrainDecal, 1)
    {"render_mode", "g"},
END_TAGS;

BEGIN_TAGS(SurfaceFX, 4)
    {"decal_name",         "H"},
    {"dynamic_track_name", "q"},
    {"soundfx_name",       "X"},
    {"status_icon_name",   "I"},
END_TAGS;

BEGIN_TAGS(DynamicTrack, 2)
    {"texture_name",    "t"},
    {"render_mode",     "g"},
END_TAGS;

BEGIN_TAGS(TacticalCamera, 0)
END_TAGS;

BEGIN_TAGS(LensFlare, 0)
END_TAGS;

BEGIN_TAGS(BlackMarketItem, 6)
    {"ability_names",           "(A)"},
    {"faction",                 "F"},
    {"icon_name",               "I"},
    {"localized_description",   "(s)"},
    {"localized_name",          "s"},
    {"used_by_units",           "(G)"},
END_TAGS;

BEGIN_TAGS(WeatherScenario, 0)
END_TAGS;

BEGIN_TAGS(WeatherModifier, 5)
    {"class_text",       "s"},
    {"description_text", "s"},
    {"display_text",     "s"},
    {"icon_name",        "I"},
    {"objective_text",   "s"},
END_TAGS;

BEGIN_TAGS(MousePointer, 1)
    {"base_texture", "t"},
END_TAGS;

BEGIN_TAGS(Goal, 3)
    {"aigoalapplicationflags", "(&)"},
    {"gamemode",               "#"},
    {"is_like",                "(!)"},
END_TAGS;

BEGIN_TAGS(Template, 1)
    {"trigger", "$"},
END_TAGS;

BEGIN_TAGS(Difficulty, 0)
END_TAGS;

BEGIN_TAGS(CommandbarComponent, 17)
    {"bar_overlay_name",            "(I)"},
    {"bar_texture_name",            "(I)"},
    {"blank_texture_name",          "(I)"},
    {"build_texture_name",          "(I)"},
    {"click_sfx",                   "X"},
    {"disabled_texture_name",       "(I)"},
    {"flash_texture_name",          "(I)"},
    {"icon_alternate_texture_name", "(I)"},
    {"icon_texture_name",           "(I)"},
    {"lower_effect_texture_name",   "(I)"},
    {"model_name",                  "m"},
    {"mouse_over_sfx",              "X"},
    {"mouse_over_texture_name",     "(I)"},
    {"overlay2_texture_name",       "(I)"},
    {"overlay_texture_name",        "(I)"},
    {"selected_texture_name",       "(I)"},
    {"tooltip_text",                "(s)"},
END_TAGS;

BEGIN_TAGS(TargetingPriority, 2)
    {"attack_priorities",   "(Cf)"},
    {"category_exclusions", "(C)"},
END_TAGS;

BEGIN_TAGS(Campaign, 18)
    {"ai_player_control",       "F@"},
    {"ai_victory_conditions",   "(7)"},
    {"description_text",        "s"},
    {"empire_story_name",       "6"},
    {"home_location",           "FG"},
    {"human_victory_conditions","(7)"},
    {"locations",               "(G)"},
    {"max_tech_level",          "Fi"},
    {"rebel_story_name",        "6"},
    {"special_case_production", "FGG"},
    {"starting_credits",        "Ff"},
    {"starting_tech_level",     "Fi"},
    {"starting_active_player",  "F"},
    {"starting_forces",         "FGG"},
    {"text_id",                 "s"},
    {"trade_routes",            "(O)"},
    {"underworld_story_name",   "6"},
    {"story_name",              "F6"},
END_TAGS;

BEGIN_TAGS(Audio, 16)
    {"music_event_battle_end_summary_screen_lose", "U"},
    {"music_event_battle_end_summary_screen_win",  "U"},
    {"music_event_battle_load_screen",             "U"},
	{"sfxevent_command_bar_attack",      "X"},
	{"sfxevent_command_bar_attack_move", "X"},
	{"sfxevent_command_bar_guard",       "X"},
	{"sfxevent_command_bar_move",        "X"},
	{"sfxevent_command_bar_stop",        "X"},
	{"sfxevent_command_bar_waypoint",    "X"},
    {"sfxevent_gui_negative_feedback",       "X"},
    {"sfxevent_mp_give_teammate_credits",    "X"},
    {"sfxevent_planet_capture_bonus_reward", "X"},
    {"sfxevent_planet_revealed",             "X"},
	{"telekinesis_sfxevent_damage",      "X"},
    {"telekinesis_sfxevent_loop",        "X"},
	{"telekinesis_sfxevent_slam",        "X"},
END_TAGS;

BEGIN_TAGS(TradeRoute, 3)
    {"point_a",             "G"},
    {"point_b",             "G"},
    {"visible_line_name",   "P"},
END_TAGS;

BEGIN_TAGS(TradeRouteLine, 1)
    {"render_mode", "g"},
END_TAGS;

BEGIN_TAGS(WeatherAudio, 3)
    {"ambient_sfxevent_intermittent", "jXff"},
    {"weather_sfxevent_intermittent", "YfXff"},
    {"weather_sfxevent_loop",         "YfX"},
END_TAGS;

BEGIN_TAGS(Hardpoint, 10)
    {"damage_type", "d"},
    {"death_breakoff_prop", "G"},
    {"death_explosion_particles", "G"},
    {"death_explosion_sfxevent", "X"},
    {"fire_inaccuracy_distance", "Cf"},
    {"fire_projectile_type", "G"},
    {"fire_sfxevent", "X"},
    {"model_to_attach", "m"},
    {"tooltip_text", "s"},
    {"type", "h"},
END_TAGS;

BEGIN_TAGS(GameConstants, 84)
    {"activated_black_market_ability_names",      "(A)"},
    {"activated_corrupt_planet_ability_names",    "(A)"},
    {"activated_destroy_planet_ability_names",    "(A)"},
    {"activated_hack_super_weapon_ability_names", "(A)"},
    {"activated_neutralize_hero_ability_names",   "(A)"},
    {"activated_remove_corruption_ability_names", "(A)"},
    {"activated_sabotage_ability_names",          "(A)"},
    {"activated_siphon_credits_ability_names",    "(A)"},
    {"activated_slice_ability_names",             "(A)"},
    {"activated_system_spy_ability_names",        "(A)"},
    {"corrupt_side_leader_name", "(G)"},
    {"corrupt_side_name",   "F"},
    {"corruption_choice_benefit",      "(s)"},
    {"corruption_choice_encyclopedia", "(s)"},
    {"corruption_choice_icon_name",    "(I)"},
    {"corruption_choice_name",         "(s)"},
    {"corruption_encyclopedia_backdrop",   "I"},
    {"corruption_encyclopedia_complete",   "I"},
    {"corruption_encyclopedia_header",     "I"},
    {"corruption_encyclopedia_incomplete", "I"},
    {"corruption_encyclopedia_money_icon", "I"},
    {"corruption_mission_requirement_icon_name", "(I)"},
    {"corruption_particle_line_name", "G"},
    {"corruption_particle_name",      "G"},
    {"corruption_planet_icon",   "(I)"},
    {"corruption_planet_icon_encyclopedia_desc", "(s)"},
    {"corruption_planet_icon_encyclopedia_name", "(s)"},
    {"credits_logo_1_name", "t"},
    {"credits_logo_2_name", "t"},
    {"credits_logo_3_name", "t"},
    {"damage_to_armor_mod", "daf"},
    {"default_bounty_by_category_mp", "Ci"},
    {"default_bounty_by_category_sp", "Ci"},
    {"demo_attract_maps",     "(e)"},
    {"energy_beam_texture",   "t"},
    {"evil_side_leader_name", "(G)"},
    {"evil_side_name",        "F"},
    {"first_strike_particle", "G"},
    {"fleet_hyperspace_band_texture_name", "t"},
    {"fleet_movement_line_texture_name", "t"},
    {"game_scoring_script_name", "Q", "Miscellaneous\\"},
    {"good_side_leader_name", "(G)"},
    {"good_side_name", "F"},
    {"gui_attack_move_command_ack_effect", "G"},
    {"gui_attack_movement_click_radar_event_name", "R"},
    {"gui_double_click_move_command_ack_effect", "G"},
    {"gui_guard_move_command_ack_effect", "G"},
    {"gui_move_command_ack_effect", "G"},
    {"gui_movement_click_radar_event_name", "R"},
    {"gui_movement_double_click_radar_event_name", "R"},
    {"hack_super_weapon_particle_effect", "G"},
    {"hack_super_weapon_required_type", "G"},
    {"hardpoint_target_reticle_enemy_texture", "ht"},
    {"hardpoint_target_reticle_enemy_tracked_texture", "ht"},
    {"hardpoint_target_reticle_friendly_disabled_texture", "ht"},
    {"hardpoint_target_reticle_friendly_disabled_tracked_texture", "ht"},
    {"hardpoint_target_reticle_friendly_repairing_texture", "ht"},
    {"hardpoint_target_reticle_friendly_texture", "ht"},
    {"hardpoint_target_reticle_friendly_tracked_texture", "ht"},
    {"localized_menu_overlay", "lt"},
    {"localized_splash_screen", "lt"},
    {"localized_uk_english_splash_screen", "t"},
    {"loopwaypointlinetexturename", "t"},
    {"planet_ability_icon_names", "(I)"},
    {"planet_ability_text_ids", "(s)"},
    {"preferred_pathfinder_types", "(G)"},
    {"raid_force_free_object_category_mask",    "(C)"},
    {"raid_force_limited_object_category_mask", "(C)"},
    {"raid_force_required_faction", "F"},
    {"random_story_empire_construction", "G"},
    {"random_story_empire_destroy", "G"},
    {"random_story_rebel_construction", "G"},
    {"random_story_rebel_destroy", "G"},
    {"random_story_reward_empire_buildable", "G"},
    {"random_story_reward_empire_unit", "G"},
    {"random_story_reward_rebel_buildable", "G"},
    {"random_story_reward_rebel_unit", "G"},
    {"random_story_rewards", "(r)"},
    {"random_story_triggers", "(S)"},
    {"sabotage_particle_effect", "G"},
    {"shipnametextfiles", "(Gn)"},
    {"tractor_beam_texture", "t"},
    {"waypointflagmodelname", "m"},
    {"waypointlinetexturename", "t"},
END_TAGS;

BEGIN_TAGS(SFXEvent, 6)
    {"chained_sfxevent",    "X"},
    {"post_samples",        "(x)"},
    {"pre_samples",         "(x)"},
    {"samples",             "(x)"},
    {"text_id",             "(s)"},
    {"use_preset",          "X"},
END_TAGS;

BEGIN_TAGS(LightningEffect, 1)
    {"texture_name", "t"},
END_TAGS;

BEGIN_TAGS(Ability, 4)
    {"activation_style",           "z"},
    {"applicable_unit_categories", "(C)"},
    {"applicable_unit_types",      "(G)"},
    {"sfxevent_target_affected",   "X"},
END_TAGS;

BEGIN_TAGS(HeroClash, 5)
    {"first_hero_type",         "G"},
    {"first_hero_win_speech",   "V"},
    {"involved_hero_types",     "(G)"},
    {"second_hero_type",        "G"},
    {"second_hero_win_speech",  "V"},
END_TAGS;

BEGIN_TAGS(TextCrawl, 3)
    {"header_text_ids", "(s)"},
    {"model_name" ,     "m"},
    {"text_ids",        "(s)"},
END_TAGS;

BEGIN_TAGS(RadarMapEvent, 1)
    {"event_model_name", "m"},
END_TAGS;

BEGIN_TAGS(RadarMapSettings, 2)
    {"land_backdrop_texture_name",  "t"},
    {"space_backdrop_texture_name", "t"},
END_TAGS;

BEGIN_TAGS(Movie, 5)
    {"caption_duration", "si"},
    {"caption_frame",    "si"},
    {"movie_file",       "n"},
    {"speechevent_frame","Vi"},
    {"text_crawl_name",  "W"},
END_TAGS;

BEGIN_TAGS(MusicEvent, 1)
    {"files", "(u)"},
END_TAGS;

BEGIN_TAGS(SpeechEvent, 2)
    {"files",   "(v)"},
    {"text_id", "(s)"},
END_TAGS;

BEGIN_TAGS(ShadowBlob, 2)
    {"render_mode",  "g"},
    {"texture_name", "t"},
END_TAGS;

BEGIN_TAGS(Faction, 213)
    {"allies",                                      "(F)"},
    {"automatic_planetary_corruption_upgrade_type", "(G)"},
    {"basic_ai",                                    "@"},
    {"bombardment_particle",                        "G"},
    {"bombardment_projectile",                      "(G)"},
    {"bombardment_required_orbital_ships",          "(G)"},
    {"bombardment_shadow_blob_material_name",       "B"},
    {"bombing_run_shadow_blob_material_name",       "B"},
    {"debug_ground_structures",                     "(G)"},
    {"default_transmission_message",                "s"},
    {"defeat_text",                                 "s"},
    {"enemies",                                     "(F)"},
    {"faction_leader",                              "G"},
    {"faction_leader_company",                      "G"},
    {"faction_super_weapon",                        "G"},
    {"finale_movie",                                "M"},
    {"finale_movie_2",                              "M"},
    {"galactic_advisor_hints",                      "(s)"},
    {"generic_win_movie",                           "M"},
    {"home_planet",                                 "G"},
    {"land_ability_targeting_range_overlay_material_name", "B"},
    {"land_advisor_hints",                          "(s)"},
    {"land_area_effect_range_overlay_material_name","B"},
    {"land_lose_image",                             "t"},
    {"land_mode_garrison_selection_blob_material_name","B"},
    {"land_mode_selection_blob_material_name",      "B"},
    {"land_retreat_begin_sfxevent",                 "X"},
    {"land_retreat_cancel_sfxevent",                "X"},
    {"land_retreat_countdown_text_id",              "s"},
    {"land_retreat_enemy_begin_sfxevent",           "X"},
    {"land_retreat_not_allowed_reason_1_sfxevent",  "X"},
    {"land_retreat_not_allowed_reason_2_sfxevent",  "X"},
    {"land_retreat_not_allowed_reason_3_sfxevent",  "X"},
    {"land_retreat_not_allowed_sfxevent",           "X"},
    {"land_skirmish_ai_default_forces",             "(G)"},
    {"land_surrender_sfxevent",                     "X"},
    {"land_win_image",                              "t"},
    {"multiplayer_beacon_type",                     "G"},
    {"multiplayer_campaign_heroes",                 "(G)"},
    {"multiplayer_map_preview_icon",                "I"},
    {"music_event_battle_load_screen",              "U"},
    {"music_event_land_ambient_super_weapon",       "U"},
    {"music_event_land_battle_super_weapon",        "U"},
    {"music_event_list_ambient",                    "(jU)"},
    {"music_event_list_battle",                     "(jU)"},
    {"music_event_space_ambient_super_weapon",      "U"},
    {"music_event_space_battle_super_weapon",       "U"},
    {"music_event_strategic_lose_vs_faction",       "FU"},
    {"music_event_strategic_win_vs_faction",        "FU"},
    {"music_event_tactical_land_battle_pending",    "U"},
    {"music_event_tactical_lose_vs_faction",        "FU"},
    {"music_event_tactical_space_battle_pending",   "U"},
    {"music_event_tactical_win_vs_faction",         "FU"},
    {"primary_enemy",                               "F"},
    {"reinforcements_ready_sfxevent",               "X"},
    {"reinforcements_requesting_sfxevent",          "X"},
    {"reinforcements_shadow_blob_material_name",    "B"},
    {"sfx_event_tactical_land_battle_pending", "X"},
    {"sfx_event_tactical_space_battle_pending", "X"},
    {"sfxevent_arrive_from_hyperspace",             "X"},
    {"sfxevent_base_shield_absorb_damage",          "X"},
    {"sfxevent_bombard_ally_available",             "X"},
    {"sfxevent_bombard_available",                  "X"},
    {"sfxevent_bombard_cancelled",                  "X"},
    {"sfxevent_bombard_enemy_available",            "X"},
    {"sfxevent_bombing_run_ally_available",         "X"},
    {"sfxevent_bombing_run_available",              "X"},
    {"sfxevent_bombing_run_begin_crosstalk",        "X"},
    {"sfxevent_bombing_run_cancelled",              "X"},
    {"sfxevent_bombing_run_enemy_available",        "X"},
    {"sfxevent_build_impossible_location_blockaded","X"},
    {"sfxevent_bunker_garrisoned",                  "X"},
    {"sfxevent_bunker_vacated",                     "X"},
    {"sfxevent_corruption_removed",                 "X"},
    {"sfxevent_enemy_fleet_approaching_planet",     "X"},
    {"sfxevent_enemy_spotted",                      "X"},
    {"sfxevent_exit_into_hyperspace",               "X"},
    {"sfxevent_gui_enemy_toggle_non_hero_ability_off", "yX"},
    {"sfxevent_gui_enemy_toggle_non_hero_ability_on",  "yX"},
    {"sfxevent_gui_start_campaign",                 "X"},
    {"sfxevent_gui_toggle_non_hero_ability_off", "yX"},
    {"sfxevent_gui_toggle_non_hero_ability_on",  "yX"},
    {"sfxevent_gui_toggle_shields_off", "X"},
    {"sfxevent_hack_success", "X"},
    {"sfxevent_hud_advisor_hint", "X"},
    {"sfxevent_hud_advisor_message", "X"},
    {"sfxevent_hud_advisor_urgent", "X"},
    {"sfxevent_hud_base_shield_offline", "X"},
    {"sfxevent_hud_base_shield_online", "X"},
    {"sfxevent_hud_base_shield_penetrated", "X"},
    {"sfxevent_hud_build_pad_captured", "X"},
    {"sfxevent_hud_build_pad_lost", "X"},
    {"sfxevent_hud_enemy_base_shield_offline", "X"},
    {"sfxevent_hud_enemy_base_shield_online", "X"},
    {"sfxevent_hud_enemy_base_shield_penetrated", "X"},
    {"sfxevent_hud_enemy_special_weapon_charging", "X"},
    {"sfxevent_hud_enemy_special_weapon_firing", "X"},
    {"sfxevent_hud_enemy_special_weapon_ready", "X"},
    {"sfxevent_hud_gravity_control_generator_off", "X"},
    {"sfxevent_hud_gravity_control_generator_on", "X"},
    {"sfxevent_hud_landing_zone_captured", "X"},
    {"sfxevent_hud_landing_zone_lost", "X"},
    {"sfxevent_hud_last_landing_zone_lost", "X"},
    {"sfxevent_hud_lost_land_battle", "X"},
    {"sfxevent_hud_lost_land_battle_enemy_tsw_present", "X"},
    {"sfxevent_hud_lost_space_battle", "X"},
    {"sfxevent_hud_lost_space_battle_enemy_tsw_present", "X"},
    {"sfxevent_hud_lost_tactical_corruption_mission", "X"},
    {"sfxevent_hud_reinforcement_point_ally_owned_05_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_ally_owned_15_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_ally_owned_30_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_ally_owned_60_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_contested", "X"},
    {"sfxevent_hud_reinforcement_point_enemy_owned_05_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_enemy_owned_15_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_enemy_owned_30_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_enemy_owned_60_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_owned_05_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_owned_15_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_owned_30_seconds", "X"},
    {"sfxevent_hud_reinforcement_point_owned_60_seconds", "X"},
    {"sfxevent_hud_repairing", "X"},
    {"sfxevent_hud_special_weapon_charging", "X"},
    {"sfxevent_hud_special_weapon_firing", "X"},
    {"sfxevent_hud_special_weapon_ready", "X"},
    {"sfxevent_hud_tactical_victory_near", "X"},
    {"sfxevent_hud_won_land_battle", "X"},
    {"sfxevent_hud_won_land_battle_enemy_tsw_present", "X"},
    {"sfxevent_hud_won_space_battle", "X"},
    {"sfxevent_hud_won_space_battle_enemy_tsw_present", "X"},
    {"sfxevent_hud_won_tactical_corruption_mission", "X"},
    {"sfxevent_land_base_under_attack_announcement", "X"},
    {"sfxevent_land_invasion_commencing", "X"},
    {"sfxevent_max_credits_limit_reached", "X"},
    {"sfxevent_mission_added", "X"},
    {"sfxevent_new_construction_options_available", "X"},
    {"sfxevent_planet_corrupted", "X"},
    {"sfxevent_planet_gained_control", "X"},
    {"sfxevent_planet_lost_control", "X"},
    {"sfxevent_player_taunt", "X"},
    {"sfxevent_sabotage_success", "X"},
    {"sfxevent_slice_failure", "X"},
    {"sfxevent_slice_success", "X"},
    {"sfxevent_space_base_under_attack_announcement", "X"},
    {"sfxevent_starbase_ally_upgraded", "X"},
    {"sfxevent_starbase_enemy_upgraded", "X"},
    {"sfxevent_starbase_upgraded", "X"},
    {"sfxevent_strategic_pop_cap_reached", "X"},
    {"sfxevent_tactical_corruption_mission_commencing", "X"},
    {"sfxevent_tactical_gain_enemy_control",        "GX"},
    {"sfxevent_tactical_gain_friendly_control",     "GX"},
    {"sfxevent_tactical_lose_enemy_control",        "GX"},
    {"sfxevent_tactical_lose_friendly_control",     "GX"},
    {"sfxevent_tactical_object_building_complete",  "X"},
    {"sfxevent_tactical_object_building_loop",      "X"},
    {"sfxevent_tactical_object_building_started",   "X"},
    {"sfxevent_tactical_object_sold", "X"},
    {"sfxevent_tactical_pop_cap_reached", "X"},
    {"sfxevent_tactical_unit_cap_reached", "X"},
    {"sfxevent_unit_type_spotted", "GX"},
    {"sfxevent_weather_begin", "YX"},
    {"sfxevent_weather_end",   "YX"},
    {"skirmish_land_bomber", "G"},
    {"space_advisor_hints", "(s)"},
    {"space_forced_retreat_due_to_superweapon", "X"},
    {"space_lose_image", "t"},
    {"space_mode_garrison_selection_blob_material_name", "B"},
    {"space_mode_selection_blob_material_name", "B"},
    {"space_retreat_begin_sfxevent", "X"},
    {"space_retreat_cancel_sfxevent", "X"},
    {"space_retreat_countdown_text_id", "s"},
    {"space_retreat_enemy_begin_sfxevent", "X"},
    {"space_retreat_not_allowed_reason_1_sfxevent", "X"},
    {"space_retreat_not_allowed_reason_2_sfxevent", "X"},
    {"space_retreat_not_allowed_reason_3_sfxevent", "X"},
    {"space_retreat_not_allowed_sfxevent", "X"},
    {"space_skirmish_ai_default_forces", "(G)"},
    {"space_surrender_sfxevent", "X"},
    {"space_win_image", "t"},
    {"speechevent_super_weapon_enemy_moved_into_range", "V"},
    {"speechevent_super_weapon_enemy_moving_into_range", "V"},
    {"speechevent_super_weapon_enemy_moving_range_05_seconds", "V"},
    {"speechevent_super_weapon_enemy_moving_range_15_seconds", "V"},
    {"speechevent_super_weapon_enemy_moving_range_30_seconds", "V"},
    {"speechevent_super_weapon_enemy_moving_range_60_seconds", "V"},
    {"speechevent_super_weapon_moved_into_range", "V"},
    {"speechevent_super_weapon_moving_into_range", "V"},
    {"speechevent_super_weapon_moving_range_05_seconds", "V"},
    {"speechevent_super_weapon_moving_range_15_seconds", "V"},
    {"speechevent_super_weapon_moving_range_30_seconds", "V"},
    {"speechevent_super_weapon_moving_range_60_seconds", "V"},
    {"speechevent_tactical_intro_land_attacker", "V"},
    {"speechevent_tactical_intro_land_attacker_conditional_or", "GGGV"},
    {"speechevent_tactical_intro_land_attacker_last_location", "V"},
    {"speechevent_tactical_intro_land_defender", "V"},
    {"speechevent_tactical_intro_land_defender_conditional_or", "GGGV"},
    {"speechevent_tactical_intro_land_defender_last_location", "V"},
    {"speechevent_tactical_intro_land_raid_attacker", "V"},
    {"speechevent_tactical_intro_land_raid_defender", "V"},
    {"speechevent_tactical_intro_space_attacker", "V"},
    {"speechevent_tactical_intro_space_attacker_conditional_and", "GGGV"},
    {"speechevent_tactical_intro_space_attacker_conditional_or", "GGGV"},
    {"speechevent_tactical_intro_space_defender", "V"},
    {"speechevent_tactical_intro_space_defender_conditional_and", "GGGV"},
    {"speechevent_tactical_intro_space_defender_conditional_or", "GGGV"},
    {"standalone_space_maps_special_weapon_a", "G"},
    {"standalone_space_maps_special_weapon_b", "G"},
    {"strategic_map_music_event", "U"},
    {"superweapon_win_movie", "M"},
    {"tactical_intro_command_bar_movie_name", "M"},
    //{"tech_tree_dialog_name", "D"},
    {"text_id", "s"},
    {"text_nickname_id", "s"},
    {"victory_text", "s"},
END_TAGS;

BEGIN_TAGS(GameObject, 253)
    {"ability_names",                               "(A)"},
    {"activate_sfx",                                "X"},
    {"activated_destroy_planet_ability_names",      "A"},
    {"additional_garrison_units",                   "(G)"},
    {"affiliation",                                 "(F)"},
    {"alternate_description_text",                  "s"},
    {"alternate_icon_name",                         "t"},
    {"alternate_name_text",                         "s"},
    {"armor_type",                                  "a"},
    {"asteroid_damage_hit_particles",               "G"},
    {"attack_category_restrictions",                "(C)"},
    {"base_shield_penetration_particle",            "G"},
    {"beacon_radar_map_event_name",                 "R"},
    {"beam_effect_name",                            "L"},
    {"beam_texture_name",                           "t"},
    {"blob_shadow_material_name",                   "B"},
    {"bomb_type",                                   "G"},
    {"bribed_effect",                               "G"},
    {"bribed_effect_small",                         "G"},
    {"build_countdown_text_id",                     "s"},
    {"build_music_completed",                       "U"},
    {"build_speech_completed",                      "V"},
    {"build_speech_stopped",                        "V"},
    {"build_speech_underway",                       "V"},
    {"cable_texture_name",                          "t"},
    {"categorymask",                                "(C)"},
    {"company_transport_unit",                      "G"},
    {"company_units",                               "(G)"},
    {"confuse_effect",                              "G"},
    {"contamination_object_name",                   "G"},
    {"conversion_ability_changes_to_enemy",         "G"},
    {"corruption_1_mission_briefing",               "s"},
    {"corruption_1_mission_commencing_speech_event","V"},
    {"corruption_1_required_unit_type",             "(G)"},
    {"corruption_1_success_bink_hologram_name",     "M"},
    {"corruption_1_success_speech_event",           "V"},
    {"corruption_1_success_unlock_unit_type",       "G"},
    {"corruption_1_tactical_map_name",              "e"},
    {"corruption_1_tactical_script_name",           "6"},
    {"corruption_2_failure_bink_hologram_name",     "M"},
    {"corruption_2_mission_briefing",               "s"},
    {"corruption_2_required_unit_type",             "G"},
    {"corruption_2_success_bink_hologram_name",     "M"},
    {"corruption_2_success_speech_event",           "V"},
    {"corruption_2_success_unlock_unit_type",       "G"},
    {"corruption_3_failure_bink_hologram_name",     "M"},
    {"corruption_3_mission_briefing",               "s"},
    {"corruption_3_required_unit_type",             "G"},
    {"corruption_3_success_bink_hologram_name",     "M"},
    {"corruption_3_success_speech_event",           "V"},
    {"create_team_type",                            "G"},
    {"damage_type",                                 "d"},
    {"damaged_smoke_asset_name",                    "p"},
    {"death_by_tsw_replacements",                   "(G)"},
    {"death_clone",                                 "dG"},
    {"death_explosions",                            "G"},
    {"death_explosions_end",                        "G"},
    {"death_projectiles",                           "G"},
    {"death_sfxevent_end_die",                      "X"},
    {"death_sfxevent_start_die",                    "X"},
    {"debris_attached_particle",                    "G"},
    {"describe_advantage",                          "s"},
    {"describe_history",                            "s"},
    {"describe_population",                         "s"},
    {"describe_tactical",                           "s"},
    {"describe_terrain",                            "s"},
    {"describe_weather",                            "s"},
    {"describe_wildlife",                           "s"},
    {"destroyed_galactic_model_name",               "m"},
    {"destruction_survivors",                       "(Gf)"},
    {"drain_effect_name",                           "G"},
    {"drain_source_bone_name",                      "I"}, // TODO: B_HAND_L{"/Drain_Source_Bone_Name>
    {"drain_target_bone_name",                      "I"}, // TODO: B_PELVIS{"/Drain_Target_Bone_Name>
    {"encyclopedia_good_against",                   "(G)"},
    {"encyclopedia_text",                           "(s)"},
    {"encyclopedia_unit_class",                     "s"},
    {"encyclopedia_vulnerable_to",                  "(G)"},
    {"encyclopedia_weather_icon",                   "I"},
    {"encyclopedia_weather_info",                   "s"},
    {"encyclopedia_weather_name",                   "s"},
    {"enemy_spawn_text",                            "(s)"},
    {"explosion_scorch_mark",                       "o"},
    {"explosion_type",                              "G"},
    {"faction_anim_subindex",                       "Fi"},
    {"flame_emitter_model_name",                    "m"},
    {"friendly_spawn_text",                         "(s)"},
    {"galactic_fleet_override_model_name",          "m"},
    {"galactic_model_name",                         "8"},
    {"glory_cinematics",                            "(k)"},
    {"gravity_control_field_effect",                "G"},
    {"grenade_type",                                "G"},
    {"gui_activated_ability_name",                  "A"},
    {"hardpoints",                                  "(J)"},
    {"heal_range_blob_material",                    "B"},
    {"highlight_blob_material_name",                "B"},
    {"icon_name",                                   "I"},
    {"ignore_for_reoptimization",                   "G"},
    {"land_bomber_type",                            "G"},
    {"land_mode_selection_blob_material_name",      "B"},
    {"land_model_anim_override_name",               "m"},
    {"land_model_name",                             "8"},
    {"land_tactical_map",                           "e"},
    {"landing_transport_variant",                   "G"},
    {"laser_beam_texture",                          "t"},
    {"lighting_effect_name",                        "L"},
    {"lightning_effect_name",                       "L"},
    {"lobbing_superweapon_chargeup_particle",       "G"},
    {"lobbing_superweapon_sfxevent_chargeup",       "X"},
    {"lobbing_superweapon_sfxevent_fire",           "X"},
    {"lua_script",                                  "Q", "GameObject\\"},
    {"marker_for_specific_object_type",             "(G)"},
    {"mine_type",                                   "G"},
    {"model_name",                                  "m"},
    {"movementclass",                               "c"},
    {"mp_encyclopedia_text",                        "(s)"},
    {"native_icon_name",                            "t"},
    {"next_level_base",                             "G"},
    {"obstacle_proxy_type",                         "G"},
    {"owner_particle_effect",                       "G"},
    {"place_other_type_at_every_specific_marker_position", "G"},
    {"planet_ability_description",                  "s"},
    {"political_faction",                           "F"},
    {"potential_indigenous_power",                  "Gi"},
    {"projectile_absorbed_by_shields_particle",     "G"},
    {"projectile_blast_area_immune_faction",        "F"},
    {"projectile_cause_invulnerability_spawn_effect","G"},
    {"projectile_combat_mod_effect",                "G"},
    {"projectile_convert_enemy_spawn_effect",       "G"},
    {"projectile_ground_detonation_surfacefx",      "o"},
    {"projectile_instant_heal_spawn_effect",        "G"},
    {"projectile_object_armor_reduced_detonation_particle","G"},
    {"projectile_object_creation_particle",         "G"},
    {"projectile_object_detonation_particle",       "G"},
    {"projectile_sfxevent_detonate",                "X"},
    {"projectile_sfxevent_detonate_reduced_by_armor","X"},
    {"projectile_stun_spawn_effect",                "G"},
    {"projectile_stun_victims_unit_types",          "(G)"},
    {"projectile_types",                            "G"},
    {"projectile_types_override",                   "G"},
    {"projectile_weaken_enemy_spawn_effect",        "G"},
    {"property_flags",                              "(2)"},
    {"radar_icon_name",                             "I"},
    {"radar_range_icon_name",                       "I"},
    {"reinforcement_region_blob_name",              "B"},
    {"reinforcements_shadow_blob_material_name",    "B"},
    {"required_orbiting_units",                     "G"},
    {"required_special_structures",                 "(G)"},
    {"reserve_spawned_units_tech_0",                "Gi"},
    {"retreat_self_destruct_explosion",             "G"},
    {"saber_name",                                  "G"},
    {"selt_destruct_sfxevent_start_die",            "X"},
    {"sfxevent_ambient_loop",                       "X"},
    {"sfxevent_ambient_moving",                     "X"},
    {"sfxevent_announce",                           "X"},
    {"sfxevent_assist_attack",                      "X"},
    {"sfxevent_assist_move",                        "X"},
    {"sfxevent_attack",                             "X"},
    {"sfxevent_attack_hardpoint",                   "hX"},
    {"sfxevent_attack_with_non_hero_ability",       "X"},
    {"sfxevent_attacked",                           "X"},
    {"sfxevent_barrage",                            "X"},
    {"sfxevent_beacon_placed",                      "X"},
    {"sfxevent_bomb_run_incoming",                  "X"},
    {"sfxevent_bomb_run_select_target",             "X"},
    {"sfxevent_bombard_incoming",                   "X"},
    {"sfxevent_bombard_select_target",              "X"},
    {"sfxevent_build_cancelled",                    "X"},
    {"sfxevent_build_complete",                     "X"},
    {"sfxevent_build_started",                      "X"},
    {"sfxevent_cough_override",                     "X"},
    {"sfxevent_damaged_by_asteroid",                "X"},
    {"sfxevent_debug_test_loop",                    "X"},
    {"sfxevent_deploy",                             "X"},
    {"sfxevent_draw_weapon",                        "X"},
    {"sfxevent_enemy_damaged_health_critical_warning","X"},
    {"sfxevent_enemy_damaged_health_low_warning",   "X"},
    {"sfxevent_enemy_health_critical_warning",      "X"},
    {"sfxevent_enemy_health_low_warning",           "X"},
    {"sfxevent_engine_cinematic_focus_loop",        "X"},
    {"sfxevent_engine_idle_loop",                   "X"},
    {"sfxevent_engine_moving_loop",                 "X"},
    {"sfxevent_fail",                               "X"},
    {"sfxevent_fire",                               "X"},
    {"sfxevent_fleet_move",                         "X"},
    {"sfxevent_group_attack",                       "X"},
    {"sfxevent_group_move",                         "X"},
    {"sfxevent_guard",                              "X"},
    {"sfxevent_gui_unit_ability_activated",         "X"},
    {"sfxevent_gui_unit_ability_deactivated",       "X"},
    {"sfxevent_hardpoint",                          "X"},
    {"sfxevent_hardpoint_all_weapons_destroyed",    "X"},
    {"sfxevent_hardpoint_destroyed",                "hX"},
    {"sfxevent_health_critical_warning",            "X"},
    {"sfxevent_health_low_warning",                 "X"},
    {"sfxevent_hero_respawned",                     "X"},
    {"sfxevent_holster_weapon",                     "X"},
    {"sfxevent_move",                               "X"},
    {"sfxevent_move_into_asteroid_field",           "X"},
    {"sfxevent_move_into_nebula",                   "X"},
    {"sfxevent_move_opposite",                      "X"},
    {"sfxevent_planet_capture_bonus_reward",        "X"},
    {"sfxevent_powered_active_loop",                "X"},
    {"sfxevent_radioactive",                        "X"},
    {"sfxevent_select",                             "X"},
    {"sfxevent_sold_tactical",                      "X"},
    {"sfxevent_special_ability_loop",               "X"},
    {"sfxevent_special_weapon_ready",               "X"},
    {"sfxevent_stop",                               "X"},
    {"sfxevent_success",                            "X"},
    {"sfxevent_tactical_build_cancelled",           "X"},
    {"sfxevent_tactical_build_complete",            "X"},
    {"sfxevent_tactical_build_started",             "X"},
    {"sfxevent_target_ability",                     "X"},
    {"sfxevent_target_affected",                    "X"},
    {"sfxevent_turret_rotating_loop",               "X"},
    {"sfxevent_unit_lost",                          "X"},
    {"sfxevent_unit_summoned",                      "X"},
    {"sfxevent_unit_under_attack",                  "X"},
    {"shield_armor_type",                           "a"},
    {"shield_hit_particles",                        "G"},
    {"space_model_name",                            "8"},
    {"space_tactical_map",                          "e"},
    {"space_upgrade_type",                          "G"},
    {"spawn_planet",                                "G"},
    {"spawned_indigenous_pack_type",                "G"},
    {"spawned_indigenous_units_type",               "(G)"},
    {"spawned_object_type",                         "G"},
    {"specific_faction",                            "F"},
    {"specific_mod_source_text",                    "s"},
    {"spin_away_on_death_explosion",                "G"},
    {"spin_away_on_death_sfxevent_start_die",       "X"},
    {"squadron_units",                              "(G)"},
    {"starting_spawned_units_tech_0",               "Gi"},
    {"stunned_effect",                              "G"},
    {"surfacefx_name",                              "(o)"},
    {"tactical_additional_structure_type",          "G"},
    {"tactical_buildable_constructed",              "G"},
    {"tactical_model_name",                         "m"},
    {"target_particle_effect",                      "G"},
    {"target_stream_source",                        "G"},
    {"targeting_priority_set",                      "w"},
    {"team_member_with_ability",                    "G"},
    {"text_id",                                     "s"},
    {"tooltip_text",                                "s"},
    {"tsw_musicevent_activation_start",             "U"},
    {"tsw_sfxevent_activate_voice",                 "X"},
    {"tsw_sfxevent_gui_bttton_press",               "X"},
    {"tsw_sfxevent_weapon_power_up",                "X"},
    {"unique_ground_unit",                          "G"},
    {"unique_space_unit",                           "G"},
    {"variant_of_existing_type",                    "G"},
    {"vehicle_thief_inside_clone",                  "G"},
    {"zap_sfxevent",                                "X"},
END_TAGS;

const TagInfo* Tags::find(const char* name) const
{
    int low = 0, high = (int)count - 1;
    while (high >= low)
    {
        int mid = (low + high) / 2;
        int cmp = _stricmp(name, list[mid].name);
        if (cmp == 0)
        {
            return &list[mid];
        }
        if (cmp < 0) high = mid - 1;
        else         low  = mid + 1;
    }
    return NULL;
}

#ifndef NDEBUG
// In debug mode, have the validation functions
static void ValidateTags(const Tags& tags)
{
    assert(tags.count > 0);
    if (tags.count > 1)
    {
        // Verify sortedness
        for (size_t i = 1; i < tags.count - 1; i++)
        {
            // Because we use _stricmp on these lists, and _stricmp does
            // lowercase comparisons, we don't want uppercase chars.
            for (const char* s = tags.list[i].name; *s != '\0'; s++)
            {
                assert(!isupper(*s));
            }

            int cmp = strcmp(tags.list[i-1].name, tags.list[i].name);
            assert(cmp < 0);
        }
    }
    // Verify that the list ends with the special marker
    assert(strcmp(tags.list[tags.count - 1].name, MAGIC_STRING) == 0);
}

void ValidateTags()
{
    ValidateTags(Tags_GameConstants);
    ValidateTags(Tags_SFXEvent);
    ValidateTags(Tags_GameObject);
    ValidateTags(Tags_MusicEvent);
    ValidateTags(Tags_ShadowBlob);
    ValidateTags(Tags_Faction);
    ValidateTags(Tags_SpeechEvent);
    ValidateTags(Tags_MusicEvent);
    ValidateTags(Tags_RadarMapEvent);
    ValidateTags(Tags_RadarMapSettings);
    ValidateTags(Tags_LightningEffect);
    ValidateTags(Tags_TextCrawl);
    ValidateTags(Tags_HeroClash);
    ValidateTags(Tags_Ability);
    ValidateTags(Tags_TerrainDecal);
    ValidateTags(Tags_SurfaceFX);
    ValidateTags(Tags_TacticalCamera);
    ValidateTags(Tags_TradeRoute);
    ValidateTags(Tags_TradeRouteLine);
    ValidateTags(Tags_Hardpoint);
    ValidateTags(Tags_TargetingPriority);
    ValidateTags(Tags_CommandbarComponent);
    ValidateTags(Tags_WeatherScenario);
    ValidateTags(Tags_WeatherModifier);
    ValidateTags(Tags_Audio);
    ValidateTags(Tags_Goal);
    ValidateTags(Tags_Template);
    ValidateTags(Tags_BlackMarketItem);
    ValidateTags(Tags_LensFlare);
    ValidateTags(Tags_None);
}
#endif