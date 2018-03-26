#include "builtins.h"
#include <cassert>
#include <cstring>
#include <cctype>

#ifndef NDEBUG
    // In debug mode, we terminate each tag list with a special marker so
    // we can validate the list at startup.
    static const char* MAGIC_STRING = "__\x41Magic__";
    #define BEGIN_BUILTIN(name, num)                                \
        extern const Builtin List_##name[num+1];                      \
        const BuiltinInfo Builtins::name = {List_##name, (num)};  \
        static const Builtin List_##name[num+1] = {
    #define END_BUILTIN MAGIC_STRING }
#else
    // In release mode, just have the plain list
    #define BEGIN_BUILTIN(name, num)                                \
        extern const Builtin List_##name[num];                        \
        const BuiltinInfo Builtins::name = {List_##name, (num)};    \
        static const Builtin List_##name[num] = {
    #define END_BUILTIN }
#endif

BEGIN_BUILTIN(HardpointTypes, 13)
    {"hard_point_dummy_art", GID_ALL},
    {"hard_point_enable_special_ability", GID_ALL},
    {"hard_point_engine", GID_ALL},
    {"hard_point_fighter_bay", GID_ALL},
    {"hard_point_gravity_well", GID_ALL},
    {"hard_point_shield_generator", GID_ALL},
    {"hard_point_tractor_beam", GID_ALL},
    {"hard_point_weapon_ion_cannon", GID_ALL},
    {"hard_point_weapon_laser", GID_ALL},
    {"hard_point_weapon_mass_driver", GID_ALL},
    {"hard_point_weapon_missile", GID_ALL},
    {"hard_point_weapon_special", GID_ALL},
    {"hard_point_weapon_torpedo", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(AIGoalApplicationFlags, 13)
    {"enemy", GID_ALL},
    {"enemy_build_pad", GID_ALL},
    {"enemy_reinforcement_point", GID_ALL},
    {"enemy_structure", GID_ALL},
    {"enemy_unit", GID_ALL},
    {"friendly", GID_ALL},
    {"friendly_build_pad", GID_ALL},
    {"friendly_reinforcement_point", GID_ALL},
    {"friendly_structure", GID_ALL},
    {"friendly_unit", GID_ALL},
    {"global", GID_ALL},
    {"neutral", GID_ALL},
    {"tactical_location", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(GameModes, 3)
    {"galactic", GID_ALL},
    {"land", GID_ALL},
    {"space", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(Difficulties, 3)
    {"easy", GID_ALL},
    {"hard", GID_ALL},
    {"normal", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(WeatherTypes, 5)
    {"ash", GID_ALL},
    {"clear", GID_ALL},
    {"rain", GID_ALL},
    {"sandstorm", GID_ALL},
    {"snow", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(VictoryConditions, 7)
    {"galactic_all_planets_controlled", GID_ALL},
    {"galactic_control_named_planets", GID_ALL},
    {"galactic_cycles_elapsed", GID_ALL},
    {"galactic_kill_enemy_leader", GID_ALL},
    {"galactic_opponent_controls_no_planets", GID_ALL},
    {"galactic_super_weapon_destroys_last_enemy_planet", GID_ALL},
    {"galactic_super_weapon_destruction", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(TerrainTypes, 8)
    {"arctic", GID_ALL},
    {"desert", GID_ALL},
    {"forest", GID_ALL},
    {"space", GID_ALL},
    {"swamp", GID_ALL},
    {"temperate", GID_ALL},
    {"urban", GID_ALL},
    {"volcanic", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(RenderModes, 5)
    {"additive", GID_ALL},
    {"alpha", GID_ALL},
    {"decal_bump_alpha", GID_ALL},
    {"diffuse_alpha", GID_ALL},
    {"modulate", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(ActivationStyles, 9)
    {"combat_automatic", GID_ALL},
    {"galactic_automatic", GID_ALL},
    {"ground_activated", GID_ALL},
    {"ground_automatic", GID_ALL},
    {"skirmish_automatic", GID_ALL},
    {"space_automatic", GID_ALL},
    {"special_attack", GID_ALL},
    {"take_damage", GID_ALL},
    {"user_input", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(AbilityClasses, 83)
    {"absorb_blaster_ability", GID_ALL},
    {"arc_sweep_attack_ability", GID_ALL},
    {"base_power_ability", GID_ALL},
    {"battlefield_modifier_ability", GID_ALL},
    {"berserker_ability", GID_ALL},
    {"black_market_ability", GID_ALL},
    {"blast_ability", GID_ALL},
    {"bounty_on_faction_ability", GID_ALL},
    {"buzz_droids_ability", GID_ALL},
    {"cable_ability", GID_ALL},
    {"cancel_special_ability", GID_ALL},
    {"cluster_bomb_ability", GID_ALL},
    {"combat_bonus_ability", GID_ALL},
    {"concentrate_fire_attack_ability", GID_ALL},
    {"corrupt_planet_ability", GID_ALL},
    {"corrupt_systems_ability", GID_ALL},
    {"corruption_ability", GID_ALL},
    {"drain_life_ability", GID_ALL},
    {"earthquake_attack_ability", GID_ALL},
    {"eat_attack_ability", GID_ALL},
    {"enable_ability", GID_ALL},
    {"enable_radar_ability", GID_ALL},
    {"energy_weapon_attack_ability", GID_ALL},
    {"enhance_defense_ability", GID_ALL},
    {"find_weakness_ability", GID_ALL},
    {"force_cloak_ability", GID_ALL},
    {"force_confuse_ability", GID_ALL},
    {"force_healing_ability", GID_ALL},
    {"force_lightning_ability", GID_ALL},
    {"force_sight_ability", GID_ALL},
    {"force_telekinesis_ability", GID_ALL},
    {"force_whirlwind_ability", GID_ALL},
    {"galactic_sabotage_ability", GID_ALL},
    {"galactic_stealth_ability", GID_ALL},
    {"galaxy_wide_upgrade_ability", GID_ALL},
    {"garrison_upgrade_ability", GID_ALL},
    {"generic_attack_ability", GID_ALL},
    {"grenade_attack_ability", GID_ALL},
    {"hack_ability", GID_ALL},
    {"hack_super_weapon_ability", GID_ALL},
    {"hero_protection_ability", GID_ALL},
    {"income_stream_ability", GID_ALL},
    {"income_stream_mod_ability", GID_ALL},
    {"infection_ability", GID_ALL},
    {"ion_cannon_shot_attack_ability", GID_ALL},
    {"laser_defense_ability", GID_ALL},
    {"leech_shields_ability", GID_ALL},
    {"lucky_shot_attack_ability", GID_ALL},
    {"maximum_firepower_attack_ability", GID_ALL},
    {"neutralize_hero_ability", GID_ALL},
    {"permanent_weapon_swap_ability", GID_ALL},
    {"personal_flame_thrower_ability", GID_ALL},
    {"planet_destruction_ability", GID_ALL},
    {"planet_income_bonus_ability", GID_ALL},
    {"planet_income_gambling_ability", GID_ALL},
    {"political_control_protection_ability", GID_ALL},
    {"political_transition_bonus_ability", GID_ALL},
    {"proximity_mines_ability", GID_ALL},
    {"radioactive_contaminate_ability", GID_ALL},
    {"redirect_blaster_ability", GID_ALL},
    {"reduce_production_price_ability", GID_ALL},
    {"reduce_production_time_ability", GID_ALL},
    {"reduce_technology_price_ability", GID_ALL},
    {"remote_bomb_ability", GID_ALL},
    {"remove_corruption_ability", GID_ALL},
    {"repair_ability", GID_ALL},
    {"retreat_prevention_ability", GID_ALL},
    {"saber_throw_ability", GID_ALL},
    {"sensor_jamming_ability", GID_ALL},
    {"shield_flare_ability", GID_ALL},
    {"siphon_credits_ability", GID_ALL},
    {"slicer_ability", GID_ALL},
    {"spawn_ability", GID_ALL},
    {"starbase_upgrade_ability", GID_ALL},
    {"stealth_ability", GID_ALL},
    {"stun_ability", GID_ALL},
    {"summon_ability", GID_ALL},
    {"super_laser_ability", GID_ALL},
    {"system_spy_ability", GID_ALL},
    {"tactical_bribe_ability", GID_ALL},
    {"tractor_beam_attack_ability", GID_ALL},
    {"vehicle_thief_ability", GID_ALL},
    {"weatherproof_ability", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(StoryTriggers, 44)
    {"story_accumulate", GID_ALL},
    {"story_ai_notification", GID_ALL},
    {"story_attack_hardpoint", GID_ALL},
    {"story_buy_black_market", GID_ALL},
    {"story_capture_structure", GID_ALL},
    {"story_check_destroyed", GID_ALL},
    {"story_click_gui", GID_ALL},
    {"story_command_unit", GID_ALL},
    {"story_conquer", GID_ALL},
    {"story_construct", GID_ALL},
    {"story_construct_level", GID_ALL},
    {"story_corruption_changed", GID_ALL},
    {"story_corruption_increased", GID_ALL},
    {"story_defeat_hero", GID_ALL},
    {"story_deploy", GID_ALL},
    {"story_destroy", GID_ALL},
    {"story_destroy_base", GID_ALL},
    {"story_elapsed", GID_ALL},
    {"story_enter", GID_ALL},
    {"story_flag", GID_ALL},
    {"story_fleet_bounced", GID_ALL},
    {"story_galactic_sabotage", GID_ALL},
    {"story_generic", GID_ALL},
    {"story_guard_unit", GID_ALL},
    {"story_invasion_bounced", GID_ALL},
    {"story_land_on", GID_ALL},
    {"story_land_tactical", GID_ALL},
    {"story_lose_battles", GID_ALL},
    {"story_mission_lost", GID_ALL},
    {"story_move", GID_ALL},
    {"story_movie_done", GID_ALL},
    {"story_open_corruption", GID_ALL},
    {"story_planet_destroyed", GID_ALL},
    {"story_select_planet", GID_ALL},
    {"story_select_unit", GID_ALL},
    {"story_space_tactical", GID_ALL},
    {"story_speech_done", GID_ALL},
    {"story_tactical_destroy", GID_ALL},
    {"story_tech_level", GID_ALL},
    {"story_trigger", GID_ALL},
    {"story_unit_proximity", GID_ALL},
    {"story_victory", GID_ALL},
    {"story_win_battles", GID_ALL},
    {"story_zoom_into_planet", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(StoryRewards, 107)
    {"add_objective", GID_ALL},
    {"bombard_delay", GID_ALL},
    {"buildable_unit", GID_ALL},
    {"change_owner", GID_ALL},
    {"commandbar_movie", GID_ALL},
    {"credits", GID_ALL},
    {"destroy_object", GID_ALL},
    {"disable_bounty_collection", GID_ALL},
    {"disable_branch", GID_ALL},
    {"disable_buildable", GID_ALL},
    {"disable_direct_invasion", GID_ALL},
    {"disable_event", GID_ALL},
    {"disable_movies", GID_ALL},
    {"disable_reinforcements", GID_ALL},
    {"disable_retreat", GID_ALL},
    {"disable_story_event", GID_ALL},
    {"disable_stroy_event", GID_ALL},
    {"enable_bounty_collection", GID_ALL},
    {"enable_buildable", GID_ALL},
    {"enable_campaign_victory_movie", GID_ALL},
    {"enable_event", GID_ALL},
    {"enable_fleet_combine", GID_ALL},
    {"enable_fow", GID_ALL},
    {"enable_galactic_corruption_hologram", GID_ALL},
    {"enable_invasion", GID_ALL},
    {"enable_objective_display", GID_ALL},
    {"enable_victory", GID_ALL},
    {"finished_tutorial", GID_ALL},
    {"flash_advanced_map_object", GID_ALL},
    {"flash_fleet_with_unit", GID_ALL},
    {"flash_gui", GID_ALL},
    {"flash_planet_gui", GID_ALL},
    {"flash_production_choice", GID_ALL},
    {"flash_special_ability", GID_ALL},
    {"force_click_gui", GID_ALL},
    {"force_respawn", GID_ALL},
    {"give_black_market", GID_ALL},
    {"hide_autoresolve", GID_ALL},
    {"hide_cursor_on_click", GID_ALL},
    {"hide_raid_slot", GID_ALL},
    {"hide_smuggle_slot", GID_ALL},
    {"hide_steal_slot", GID_ALL},
    {"hide_tutorial_cursor", GID_ALL},
    {"highlight_object", GID_ALL},
    {"increment_flag", GID_ALL},
    {"invade_planet", GID_ALL},
    {"link_tactical", GID_ALL},
    {"load_campaign", GID_ALL},
    {"lock_controls", GID_ALL},
    {"lock_planet_selection", GID_ALL},
    {"lock_unit", GID_ALL},
    {"move_fleet", GID_ALL},
    {"multimedia", GID_ALL},
    {"new_power_for_all", GID_ALL},
    {"objective_complete", GID_ALL},
    {"objective_failed", GID_ALL},
    {"planet_faction", GID_ALL},
    {"position_camera", GID_ALL},
    {"remove_all_objectives", GID_ALL},
    {"remove_objective", GID_ALL},
    {"remove_power_from_all", GID_ALL},
    {"remove_story_goal", GID_ALL},
    {"remove_unit", GID_ALL},
    {"reset_branch", GID_ALL},
    {"reset_galactic_filters", GID_ALL},
    {"restrict_autoresolve", GID_ALL},
    {"restrict_corruption", GID_ALL},
    {"restrict_sabotage", GID_ALL},
    {"reveal_planet", GID_ALL},
    {"sabotage_structure", GID_ALL},
    {"screen_text", GID_ALL},
    {"scroll_camera", GID_ALL},
    {"scroll_lock", GID_ALL},
    {"select_planet", GID_ALL},
    {"set_advisor", GID_ALL},
    {"set_flag", GID_ALL},
    {"set_max_tech_level", GID_ALL},
    {"set_planet_restricted", GID_ALL},
    {"set_planet_spawn", GID_ALL},
    {"set_planet_visibility_level", GID_ALL},
    {"set_sandbox_objectives", GID_ALL},
    {"set_tech_level", GID_ALL},
    {"show_command_bar", GID_ALL},
    {"show_raid_slot", GID_ALL},
    {"show_smuggle_slot", GID_ALL},
    {"show_special_slot", GID_ALL},
    {"show_steal_slot", GID_ALL},
    {"skirmish_rules", GID_ALL},
    {"spawn_hero", GID_ALL},
    {"speech", GID_ALL},
    {"start_cinematic_mode", GID_ALL},
    {"start_movie", GID_ALL},
    {"stop_cinematic_mode", GID_ALL},
    {"stop_commandbar_movie", GID_ALL},
    {"story_element", GID_ALL},
    {"story_goal_completed", GID_ALL},
    {"switch_control", GID_ALL},
    {"trigger_ai", GID_ALL},
    {"trigger_event", GID_ALL},
    {"tutorial_dialog", GID_ALL},
    {"tutorial_player", GID_ALL},
    {"unique_unit", GID_ALL},
    {"update_objective", GID_ALL},
    {"use_retry_dialog", GID_ALL},
    {"victory", GID_ALL},
    {"zoom_in", GID_ALL},
    {"zoom_out", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(Languages, 11)
    {"chinese", GID_ALL},
    {"english", GID_ALL},
    {"french", GID_ALL},
    {"german", GID_ALL},
    {"italian", GID_ALL},
    {"japanese", GID_ALL},
    {"korean", GID_ALL},
    {"polish", GID_ALL},
    {"russian", GID_ALL},
    {"spanish", GID_ALL},
    {"thai", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(AbilityTypes, 71)
    {"area_effect_convert", GID_ALL},
    {"area_effect_heal", GID_ALL},
    {"area_effect_stun", GID_ALL},
    {"barrage", GID_ALL},
    {"berserker", GID_ALL},
    {"blast", GID_ALL},
    {"buzz_droids", GID_ALL},
    {"cable_attack", GID_ALL},
    {"capture_vehicle", GID_ALL},
    {"cluster_bomb", GID_ALL},
    {"concentrate_fire", GID_ALL},
    {"corrupt_systems", GID_ALL},
    {"defend", GID_ALL},
    {"deploy", GID_ALL},
    {"deploy_squad", GID_ALL},
    {"deploy_troopers", GID_ALL},
    {"detonate_remote_bomb", GID_ALL},
    {"drain_life", GID_ALL},
    {"eject_vehicle_thief", GID_ALL},
    {"energy_weapon", GID_ALL},
    {"fire_lobbing_superweapon", GID_ALL},
    {"flame_thrower", GID_ALL},
    {"force_cloak", GID_ALL},
    {"force_confuse", GID_ALL},
    {"force_lightning", GID_ALL},
    {"force_sight", GID_ALL},
    {"force_telekinesis", GID_ALL},
    {"force_whirlwind", GID_ALL},
    {"fow_reveal_ping", GID_ALL},
    {"full_salvo", GID_ALL},
    {"harmonic_bomb", GID_ALL},
    {"hunt", GID_ALL},
    {"infection", GID_ALL},
    {"interdict", GID_ALL},
    {"invulnerability", GID_ALL},
    {"ion_cannon_shot", GID_ALL},
    {"jet_pack", GID_ALL},
    {"laser_defense", GID_ALL},
    {"leech_shields", GID_ALL},
    {"lucky_shot", GID_ALL},
    {"lure", GID_ALL},
    {"maximum_firepower", GID_ALL},
    {"missile_shield", GID_ALL},
    {"place_remote_bomb", GID_ALL},
    {"power_to_weapons", GID_ALL},
    {"proximity_mines", GID_ALL},
    {"radioactive_contaminate", GID_ALL},
    {"replenish_wingmen", GID_ALL},
    {"rocket_attack", GID_ALL},
    {"saber_throw", GID_ALL},
    {"self_destruct", GID_ALL},
    {"sensor_jamming", GID_ALL},
    {"shield_flare", GID_ALL},
    {"spoiler_lock", GID_ALL},
    {"spread_out", GID_ALL},
    {"sprint", GID_ALL},
    {"stealth", GID_ALL},
    {"sticky_bomb", GID_ALL},
    {"stim_pack", GID_ALL},
    {"stun", GID_ALL},
    {"summon", GID_ALL},
    {"super_laser", GID_ALL},
    {"swap_weapons", GID_ALL},
    {"tactical_bribe", GID_ALL},
    {"targeted_hack", GID_ALL},
    {"targeted_invulnerability", GID_ALL},
    {"targeted_repair", GID_ALL},
    {"tractor_beam", GID_ALL},
    {"turbo", GID_ALL},
    {"untargeted_sticky_bomb", GID_ALL},
    {"weaken_enemy", GID_ALL},
END_BUILTIN;

BEGIN_BUILTIN(MousePointers, 53)
    {"pointer_attack", GID_ALL},
    {"pointer_attack_move", GID_ALL},
    {"pointer_attack_only_mode_no_unit_targeted", GID_ALL},
    {"pointer_attack_only_mode_unit_targeted", GID_ALL},
    {"pointer_attack_out_of_range", GID_ALL},
    {"pointer_attack_with_special_weapon_0", GID_ALL},
    {"pointer_attack_with_special_weapon_1", GID_ALL},
    {"pointer_attack_with_special_weapon_2", GID_ALL},
    {"pointer_attack_with_special_weapon_invalid", GID_ALL},
    {"pointer_barrage", GID_ALL},
    {"pointer_bombing_run_target", GID_ALL},
    {"pointer_can_grab", GID_ALL},
    {"pointer_cant_attack", GID_ALL},
    {"pointer_cant_combine", GID_ALL},
    {"pointer_cant_drop", GID_ALL},
    {"pointer_cant_grab", GID_ALL},
    {"pointer_cant_move", GID_ALL},
    {"pointer_cant_select", GID_ALL},
    {"pointer_combine", GID_ALL},
    {"pointer_drag", GID_ALL},
    {"pointer_drag_select_mode", GID_ALL},
    {"pointer_drop", GID_ALL},
    {"pointer_garrison_in", GID_EAW_FOC},
    {"pointer_garrison_out", GID_EAW_FOC},
    {"pointer_grabbing", GID_ALL},
    {"pointer_guard", GID_ALL},
    {"pointer_map_rotating_mode", GID_ALL},
    {"pointer_map_scrolling_mode", GID_ALL},
    {"pointer_move", GID_ALL},
    {"pointer_move_only_mode_no_passable_targeted", GID_ALL},
    {"pointer_move_only_mode_passable_targeted", GID_ALL},
    {"pointer_movie_mode", GID_ALL},
    {"pointer_normal", GID_ALL},
    {"pointer_place_beacon", GID_ALL},
    {"pointer_reinforcements_landing_point", GID_ALL},
    {"pointer_repair_hardpoint", GID_EAW_FOC},
    {"pointer_select", GID_ALL},
    {"pointer_tactical_build", GID_ALL},
    {"pointer_tactical_sell", GID_ALL},
    {"pointer_target_special_ability_blast", GID_EAW_FOC},
    {"pointer_target_special_ability_cant_bribe_enemy_object", GID_EAW_FOC},
    {"pointer_target_special_ability_to_bribe_enemy_object", GID_EAW_FOC},
    {"pointer_target_special_ability_to_enemy_object", GID_ALL},
    {"pointer_target_special_ability_to_enemy_object_invalid", GID_ALL},
    {"pointer_target_special_ability_to_friendly_object", GID_ALL},
    {"pointer_target_special_ability_to_friendly_object_invalid", GID_ALL},
    {"pointer_target_special_ability_to_passable_terrain", GID_ALL},
    {"pointer_target_special_ability_to_passable_terrain_invalid", GID_ALL},
    {"pointer_target_special_ability_to_space_position", GID_ALL},
    {"pointer_target_special_ability_to_space_position_invalid", GID_ALL},
    {"pointer_wait", GID_ALL},
    {"pointer_waypoint_follow", GID_ALL},
    {"pointer_waypoint_placement", GID_ALL},
END_BUILTIN;

#ifndef NDEBUG
// In debug mode, have the validation functions
static void ValidateBuiltin(const BuiltinInfo& info)
{
    assert(info.count > 0);
    if (info.count > 1)
    {
        // Verify sortedness
        for (size_t i = 1; i < info.count; i++)
        {
            // Because we use _stricmp on these lists, and _stricmp does
            // lowercase comparisons, we don't want uppercase chars.
            for (const char* s = info.list[i].name; *s != '\0'; s++)
            {
                assert(!isupper(*s));
            }

            int cmp = strcmp(info.list[i-1].name, info.list[i].name);
            assert(cmp < 0);
        }
    }
    // Verify that the list ends with the special marker
    assert(strcmp(info.list[info.count].name, MAGIC_STRING) == 0);
}

void ValidateBuiltins()
{
    ValidateBuiltin(Builtins::HardpointTypes);
    ValidateBuiltin(Builtins::StoryTriggers);
    ValidateBuiltin(Builtins::StoryRewards);
    ValidateBuiltin(Builtins::Languages);
    ValidateBuiltin(Builtins::AbilityTypes);
    ValidateBuiltin(Builtins::ActivationStyles);
    ValidateBuiltin(Builtins::AbilityClasses);
    ValidateBuiltin(Builtins::TerrainTypes);
    ValidateBuiltin(Builtins::RenderModes);
    ValidateBuiltin(Builtins::WeatherTypes);
    ValidateBuiltin(Builtins::MousePointers);
    ValidateBuiltin(Builtins::VictoryConditions);
    ValidateBuiltin(Builtins::GameModes);
    ValidateBuiltin(Builtins::Difficulties);
}
#endif