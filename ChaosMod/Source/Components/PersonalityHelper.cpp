#include "PCH.hpp"

#include "Components/PersonalityHelper.hpp"

std::optional<const pub::AI::Personality*> PersonalityHelper::Get(const std::string& pilotNickname)
{
    const auto& pilot = pilots.find(pilotNickname);
    if (pilot == pilots.end())
    {
        return std::nullopt;
    }

    return &pilot->second;
}

void PersonalityHelper::SetDirection(INI_Reader& ini, float (&direction)[4])
{
    const auto str = std::string(ini.get_value_string(0));
    if (str == "right")
    {
        direction[0] = ini.get_value_float(1);
    }
    else if (str == "left")
    {
        direction[1] = ini.get_value_float(1);
    }
    else if (str == "up")
    {
        direction[2] = ini.get_value_float(1);
    }
    else if (str == "down")
    {
        direction[3] = ini.get_value_float(1);
    }
}

void PersonalityHelper::LoadEvadeDodge(INI_Reader& ini)
{
    pub::AI::Personality::EvadeDodgeUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("evade_dodge_style_weight"))
        {
            const auto str = std::string(ini.get_value_string(0));
            if (str == "waggle")
            {
                data.evade_dodge_style_weight[0] = ini.get_value_float(1);
            }
            else if (str == "waggle_random")
            {
                data.evade_dodge_style_weight[1] = ini.get_value_float(1);
            }
            else if (str == "slide")
            {
                data.evade_dodge_style_weight[2] = ini.get_value_float(1);
            }
            else if (str == "corkscrew")
            {
                data.evade_dodge_style_weight[3] = ini.get_value_float(1);
            }
        }
        else if (ini.is_value("evade_dodge_direction_weight"))
        {
            SetDirection(ini, data.evade_dodge_direction_weight);
        }
        else if (ini.is_value("evade_dodge_cone_angle"))
        {
            data.evade_dodge_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_interval_time"))
        {
            data.evade_dodge_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_time"))
        {
            data.evade_dodge_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_distance"))
        {
            data.evade_dodge_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_activate_range"))
        {
            data.evade_activate_range = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_roll_angle"))
        {
            data.evade_dodge_roll_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_waggle_axis_cone_angle"))
        {
            data.evade_dodge_waggle_axis_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_slide_throttle"))
        {
            data.evade_dodge_slide_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_turn_throttle"))
        {
            data.evade_dodge_turn_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_corkscrew_turn_throttle"))
        {
            data.evade_dodge_corkscrew_turn_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_corkscrew_roll_throttle"))
        {
            data.evade_dodge_corkscrew_roll_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_corkscrew_roll_flip_direction"))
        {
            data.evade_dodge_corkscrew_roll_flip_direction = ini.get_value_bool(0);
        }
        else if (ini.is_value("evade_dodge_interval_time_variance_percent"))
        {
            data.evade_dodge_interval_time_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_cone_angle_variance_percent"))
        {
            data.evade_dodge_cone_angle_variance_percent = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        evadeDodge[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadEvadeBreak(INI_Reader& ini)
{
    pub::AI::Personality::EvadeBreakUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("evade_break_roll_throttle"))
        {
            data.evade_break_roll_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_time"))
        {
            data.evade_break_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_interval_time"))
        {
            data.evade_break_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_afterburner_delay"))
        {
            data.evade_break_afterburner_delay = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_turn_throttle"))
        {
            data.evade_break_turn_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_direction_weight"))
        {
            SetDirection(ini, data.evade_break_direction_weight);
        }
        else if (ini.is_value("evade_break_roll_throttle"))
        {
            const auto str = std::string(ini.get_value_string(0));
            if (str == "sideways")
            {
                data.evade_break_style_weight[0] = ini.get_value_float(1);
            }
            else if (str == "outrun")
            {
                data.evade_break_style_weight[1] = ini.get_value_float(1);
            }
            else if (str == "reverse")
            {
                data.evade_break_style_weight[2] = ini.get_value_float(1);
            }
        }
        else if (ini.is_value("evade_break_attempt_reverse_time"))
        {
            data.evade_break_attempt_reverse_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_reverse_distance"))
        {
            data.evade_break_reverse_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_afterburner_delay_variance_percent"))
        {
            data.evade_break_afterburner_delay_variance_percent = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        evadeBreak[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadBuzzHead(INI_Reader& ini)
{
    pub::AI::Personality::BuzzHeadTowardUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("buzz_dodge_cone_angle"))
        {
            data.buzz_dodge_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_dodge_cone_angle_variance_percent"))
        {
            data.buzz_dodge_cone_angle_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_dodge_interval_time"))
        {
            data.buzz_dodge_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_dodge_interval_time_variance_percent"))
        {
            data.buzz_dodge_interval_time_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_dodge_roll_angle"))
        {
            data.buzz_dodge_roll_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_dodge_direction_weight"))
        {
            SetDirection(ini, data.buzz_dodge_direction_weight);
        }
        else if (ini.is_value("buzz_head_toward_style_weight"))
        {
            const auto str = std::string(ini.get_value_string(0));
            if (str == "straight_to")
            {
                data.buzz_head_toward_style_weight[0] = ini.get_value_float(1);
            }
            else if (str == "slide")
            {
                data.buzz_head_toward_style_weight[1] = ini.get_value_float(1);
            }
            else if (str == "waggle")
            {
                data.buzz_head_toward_style_weight[2] = ini.get_value_float(1);
            }
        }
        else if (ini.is_value("buzz_dodge_turn_throttle"))
        {
            data.buzz_dodge_turn_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_dodge_waggle_axis_cone_angle"))
        {
            data.buzz_dodge_waggle_axis_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_head_toward_engine_throttle"))
        {
            data.buzz_head_toward_engine_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_head_toward_roll_flip_direction"))
        {
            data.buzz_head_toward_roll_flip_direction = ini.get_value_bool(0);
        }
        else if (ini.is_value("buzz_head_toward_roll_throttle"))
        {
            data.buzz_head_toward_roll_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_slide_throttle"))
        {
            data.buzz_slide_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_slide_interval_time_variance_percent"))
        {
            data.buzz_slide_interval_time_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_slide_interval_time"))
        {
            data.buzz_slide_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_min_distance_to_head_toward"))
        {
            data.buzz_min_distance_to_head_toward = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_min_distance_to_head_toward_variance_percent"))
        {
            data.buzz_min_distance_to_head_toward_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_max_time_to_head_away"))
        {
            data.buzz_max_time_to_head_away = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_head_toward_turn_throttle"))
        {
            data.buzz_head_toward_turn_throttle = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        buzzHead[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadBuzzPass(INI_Reader& ini)
{
    pub::AI::Personality::BuzzPassByUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("buzz_break_direction_cone_angle"))
        {
            data.buzz_break_direction_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_break_turn_throttle"))
        {
            data.buzz_break_turn_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_distance_to_pass_by"))
        {
            data.buzz_distance_to_pass_by = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_drop_bomb_on_pass_by"))
        {
            data.buzz_drop_bomb_on_pass_by = ini.get_value_bool(0);
        }
        else if (ini.is_value("buzz_pass_by_roll_throttle"))
        {
            data.buzz_pass_by_roll_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_break_direction_weight"))
        {
            SetDirection(ini, data.buzz_break_direction_weight);
        }
        else if (ini.is_value("evade_break_roll_throttle"))
        {
            const auto str = std::string(ini.get_value_string(0));
            if (str == "straight_by")
            {
                data.buzz_pass_by_style_weight[0] = ini.get_value_float(1);
            }
            else if (str == "break_away")
            {
                data.buzz_pass_by_style_weight[1] = ini.get_value_float(1);
            }
            else if (str == "engine_kill")
            {
                data.buzz_pass_by_style_weight[2] = ini.get_value_float(1);
            }
        }
        else if (ini.is_value("buzz_pass_by_roll_throttle"))
        {
            data.buzz_pass_by_roll_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("buzz_pass_by_time"))
        {
            data.buzz_pass_by_time = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        buzzPass[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadTrail(INI_Reader& ini)
{
    pub::AI::Personality::TrailUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("trail_break_afterburner"))
        {
            data.trail_break_afterburner = ini.get_value_bool(0);
        }
        else if (ini.is_value("trail_break_roll_throttle"))
        {
            data.trail_break_roll_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("trail_break_time"))
        {
            data.trail_break_time = ini.get_value_float(0);
        }
        else if (ini.is_value("trail_distance"))
        {
            data.trail_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("trail_lock_cone_angle"))
        {
            data.trail_lock_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("trail_max_turn_throttle"))
        {
            data.trail_max_turn_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("trail_min_no_lock_time"))
        {
            data.trail_min_no_lock_time = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        trail[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadStrafe(INI_Reader& ini)
{
    pub::AI::Personality::StrafeUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("strafe_attack_throttle"))
        {
            data.strafe_attack_throttle = ini.get_value_bool(0);
        }
        else if (ini.is_value("strafe_run_away_distance"))
        {
            data.strafe_run_away_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("strafe_turn_throttle"))
        {
            data.strafe_turn_throttle = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        strafe[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadEngineKill(INI_Reader& ini)
{
    pub::AI::Personality::EngineKillUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("engine_kill_afterburner_time"))
        {
            data.engine_kill_afterburner_time = ini.get_value_float(0);
        }
        else if (ini.is_value("engine_kill_face_time"))
        {
            data.engine_kill_face_time = ini.get_value_float(0);
        }
        else if (ini.is_value("engine_kill_max_target_distance"))
        {
            data.engine_kill_max_target_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("engine_kill_search_time"))
        {
            data.engine_kill_search_time = ini.get_value_float(0);
        }
        else if (ini.is_value("engine_kill_use_afterburner"))
        {
            data.engine_kill_use_afterburner = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        engineKill[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadRepair(INI_Reader& ini)
{
    pub::AI::Personality::RepairUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("use_hull_repair_at_damage_percent"))
        {
            data.use_hull_repair_at_damage_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("use_hull_repair_post_delay"))
        {
            data.use_hull_repair_post_delay = ini.get_value_float(0);
        }
        else if (ini.is_value("use_hull_repair_pre_delay"))
        {
            data.use_hull_repair_pre_delay = ini.get_value_float(0);
        }
        else if (ini.is_value("use_shield_repair_at_damage_percent"))
        {
            data.use_shield_repair_at_damage_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("use_shield_repair_post_delay"))
        {
            data.use_shield_repair_post_delay = ini.get_value_float(0);
        }
        else if (ini.is_value("use_shield_repair_pre_delay"))
        {
            data.use_shield_repair_pre_delay = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        repair[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadGun(INI_Reader& ini)
{
    pub::AI::Personality::GunUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("gun_fire_accuracy_cone_angle"))
        {
            data.gun_fire_accuracy_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_fire_accuracy_power"))
        {
            data.gun_fire_accuracy_power = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_fire_accuracy_power_npc"))
        {
            data.gun_fire_accuracy_power_npc = ini.get_value_float(0);
        }
        else if (ini.is_value("use_shield_repair_at_damage_percent"))
        {
            data.gun_fire_burst_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_fire_burst_interval_variance_percent"))
        {
            data.gun_fire_burst_interval_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_fire_interval_time"))
        {
            data.gun_fire_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_fire_burst_interval_variance_percent"))
        {
            data.gun_fire_burst_interval_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_fire_no_burst_interval_time"))
        {
            data.gun_fire_no_burst_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_fire_interval_variance_percent"))
        {
            data.gun_fire_interval_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_range_threshold"))
        {
            data.gun_range_threshold = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_range_threshold_variance_percent"))
        {
            data.gun_range_threshold_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("gun_target_point_switch_time"))
        {
            data.gun_target_point_switch_time = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        gun[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadMine(INI_Reader& ini)
{
    pub::AI::Personality::MineUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("mine_launch_cone_angle"))
        {
            data.mine_launch_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("mine_launch_interval"))
        {
            data.mine_launch_interval = ini.get_value_float(0);
        }
        else if (ini.is_value("mine_launch_range"))
        {
            data.mine_launch_range = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        mine[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadMissileReaction(INI_Reader& ini)
{
    pub::AI::Personality::MissileReactionStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("evade_afterburn_missile_reaction_time"))
        {
            data.evade_afterburn_missile_reaction_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_missile_reaction_time"))
        {
            data.evade_break_missile_reaction_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_missile_distance"))
        {
            data.evade_missile_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_slide_missile_reaction_time"))
        {
            data.evade_slide_missile_reaction_time = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        missileReaction[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadDamageReaction(INI_Reader& ini)
{
    pub::AI::Personality::DamageReactionStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("afterburner_damage_trigger_percent"))
        {
            data.afterburner_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("afterburner_damage_trigger_time"))
        {
            data.afterburner_damage_trigger_time = ini.get_value_float(0);
        }
        else if (ini.is_value("brake_reverse_damage_trigger_percent"))
        {
            data.brake_reverse_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("drop_mines_damage_trigger_percent"))
        {
            data.drop_mines_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("drop_mines_damage_trigger_time"))
        {
            data.drop_mines_damage_trigger_time = ini.get_value_float(0);
        }
        else if (ini.is_value("engine_kill_face_damage_trigger_percent"))
        {
            data.engine_kill_face_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("engine_kill_face_damage_trigger_time"))
        {
            data.engine_kill_face_damage_trigger_time = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_break_damage_trigger_percent"))
        {
            data.evade_break_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("evade_dodge_more_damage_trigger_percent"))
        {
            data.evade_dodge_more_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("fire_guns_damage_trigger_percent"))
        {
            data.fire_guns_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("fire_guns_damage_trigger_time"))
        {
            data.fire_guns_damage_trigger_time = ini.get_value_float(0);
        }
        else if (ini.is_value("fire_missiles_damage_trigger_percent"))
        {
            data.fire_missiles_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("fire_missiles_damage_trigger_time"))
        {
            data.fire_missiles_damage_trigger_time = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        damageReaction[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadCM(INI_Reader& ini)
{
    pub::AI::Personality::CountermeasureUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("countermeasure_active_time"))
        {
            data.countermeasure_active_time = ini.get_value_float(0);
        }
        else if (ini.is_value("countermeasure_unactive_time"))
        {
            data.countermeasure_unactive_time = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        cm[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadFormation(INI_Reader& ini)
{
    pub::AI::Personality::FormationUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("break_apart_formation_damage_trigger_percent"))
        {
            data.break_apart_formation_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("break_apart_formation_damage_trigger_time"))
        {
            data.break_apart_formation_damage_trigger_time = ini.get_value_float(0);
        }
        else if (ini.is_value("break_apart_formation_missile_reaction_time"))
        {
            data.break_apart_formation_missile_reaction_time = ini.get_value_float(0);
        }
        else if (ini.is_value("break_apart_formation_on_buzz_head_toward"))
        {
            data.break_apart_formation_on_buzz_head_toward = ini.get_value_bool(0);
        }
        else if (ini.is_value("break_apart_formation_on_buzz_head_toward"))
        {
            data.break_apart_formation_on_buzz_head_toward = ini.get_value_bool(0);
        }
        else if (ini.is_value("break_apart_formation_on_buzz_pass_by"))
        {
            data.break_apart_formation_on_buzz_pass_by = ini.get_value_bool(0);
        }
        else if (ini.is_value("break_apart_formation_on_evade_break"))
        {
            data.break_apart_formation_on_evade_break = ini.get_value_bool(0);
        }
        else if (ini.is_value("break_apart_formation_on_evade_dodge"))
        {
            data.break_apart_formation_on_evade_dodge = ini.get_value_bool(0);
        }
        else if (ini.is_value("break_formation_damage_trigger_percent"))
        {
            data.break_formation_damage_trigger_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("break_formation_damage_trigger_time"))
        {
            data.break_formation_damage_trigger_time = ini.get_value_float(0);
        }
        else if (ini.is_value("break_formation_missile_reaction_time"))
        {
            data.break_formation_missile_reaction_time = ini.get_value_float(0);
        }
        else if (ini.is_value("break_formation_on_buzz_head_toward_time"))
        {
            data.break_formation_on_buzz_head_toward_time = ini.get_value_float(0);
        }
        else if (ini.is_value("formation_exit_top_turn_break_away_throttle"))
        {
            data.formation_exit_top_turn_break_away_throttle = ini.get_value_float(0);
        }
        else if (ini.is_value("regroup_formation_on_buzz_head_toward"))
        {
            data.regroup_formation_on_buzz_head_toward = ini.get_value_bool(0);
        }
        else if (ini.is_value("regroup_formation_on_buzz_pass_by"))
        {
            data.regroup_formation_on_buzz_pass_by = ini.get_value_bool(0);
        }
        else if (ini.is_value("regroup_formation_on_evade_break"))
        {
            data.regroup_formation_on_evade_break = ini.get_value_bool(0);
        }
        else if (ini.is_value("regroup_formation_on_evade_dodge"))
        {
            data.regroup_formation_on_evade_dodge = ini.get_value_bool(0);
        }
        else if (ini.is_value("force_attack_formation_active_time"))
        {
            data.force_attack_formation_active_time = ini.get_value_float(0);
        }
        else if (ini.is_value("force_attack_formation_unactive_time"))
        {
            data.force_attack_formation_unactive_time = ini.get_value_float(0);
        }
        else if (ini.is_value("leader_makes_me_tougher"))
        {
            data.leader_makes_me_tougher = ini.get_value_bool(0);
        }
        else if (ini.is_value("formation_exit_max_time"))
        {
            data.formation_exit_max_time = ini.get_value_float(0);
        }
        else if (ini.is_value("formation_exit_mode"))
        {
            data.formation_exit_mode = ini.get_value_int(0);
        }
        else if (ini.is_value("formation_exit_roll_outrun_throttle"))
        {
            data.formation_exit_roll_outrun_throttle = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        formation[std::move(nick)] = data;
    }
}

void PersonalityHelper::GetDifficulty(INI_Reader& ini, int& difficulty)
{
    if (const std::string str = StringUtils::ToLower(std::string(ini.get_value_string(0))); str == "easiest")
    {
        difficulty = 0;
    }
    else if (str == "easy")
    {
        difficulty = 1;
    }
    else if (str == "equal")
    {
        difficulty = 2;
    }
    else if (str == "hard")
    {
        difficulty = 3;
    }
    else
    {
        difficulty = 4;
    }
}

void PersonalityHelper::LoadJob(INI_Reader& ini)
{
    pub::AI::Personality::JobStruct data;
    std::string nick;
    uint attackPrefCounter = 0;
    uint attackSubTargetCounter = 0;

    // attack_preference = anything, 5000, GUNS | GUIDED | UNGUIDED

    // Missing Attack Order / Attack Sub target - more complex
    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("allow_player_targeting"))
        {
            data.allow_player_targeting = StringUtils::ToLower(std::string(ini.get_value_string())) == "true";
        }
        else if (ini.is_value("force_attack_formation"))
        {
            data.force_attack_formation = StringUtils::ToLower(std::string(ini.get_value_string())) == "true";
        }
        else if (ini.is_value("force_attack_formation_used"))
        {
            data.force_attack_formation_used = StringUtils::ToLower(std::string(ini.get_value_string())) == "true";
        }
        else if (ini.is_value("combat_drift_distance"))
        {
            data.combat_drift_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("flee_scene_threat_style"))
        {
            GetDifficulty(ini, data.flee_scene_threat_style);
        }
        else if (ini.is_value("field_targeting"))
        {
            const std::string str = StringUtils::ToLower(std::string(ini.get_value_string(0)));
            if (str == "never")
            {
                data.field_targeting = 0;
            }
            else if (str == "low_density")
            {
                data.field_targeting = 1;
            }
            else if (str == "high_density")
            {
                data.field_targeting = 2;
            }
            else if (str == "always")
            {
                data.field_targeting = 3;
            }
        }
        else if (ini.is_value("flee_no_weapons_style"))
        {
            data.flee_no_weapons_style = StringUtils::ToLower(std::string(ini.get_value_string())) == "true";
        }
        else if (ini.is_value("target_toughness_preference"))
        {
            GetDifficulty(ini, data.target_toughness_preference);
        }
        else if (ini.is_value("loot_flee_threshold"))
        {
            GetDifficulty(ini, data.loot_flee_threshold);
        }
        else if (ini.is_value("wait_for_leader_target"))
        {
            data.wait_for_leader_target = ini.get_value_bool(0);
        }
        else if (ini.is_value("flee_when_leader_flees_style"))
        {
            data.flee_when_leader_flees_style = ini.get_value_bool(0);
        }
        else if (ini.is_value("maximum_leader_target_distance"))
        {
            data.maximum_leader_target_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("flee_when_hull_damaged_percent"))
        {
            data.flee_when_hull_damaged_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("loot_preference"))
        {
            const std::string str = ini.get_value_string(0);
            if (str.find("LT_COMMODITIES"))
            {
                data.loot_preference |= 1;
            }
            if (str.find("LT_EQUIPMENT"))
            {
                data.loot_preference |= 2;
            }
            if (str.find("LT_POTIONS"))
            {
                data.loot_preference |= 4;
            }
            if (str.find("LT_ALL"))
            {
                data.loot_preference = 7;
            }
            if (str.find("LT_NONE"))
            {
                data.loot_preference = 0;
            }
        }
        else if (ini.is_value("scene_toughness_threshold"))
        {
            GetDifficulty(ini, data.scene_toughness_threshold);
        }
        else if (ini.is_value("attack_subtarget_order"))
        {

            std::string val = StringUtils::ToLower(std::string(ini.get_value_string(0)));
            int flag;
            if (val == "anything")
            {
                flag = 6;
            }
            else if (val == "guns")
            {
                flag = 0;
            }
            else if (val == "turrets")
            {
                flag = 1;
            }
            else if (val == "launchers")
            {
                flag = 2;
            }
            else if (val == "towers")
            {
                flag = 3;
            }
            else if (val == "engines")
            {
                flag = 4;
            }
            else if (val == "hull")
            {
                flag = 5;
            }

            data.attack_subtarget_order[attackPrefCounter] = flag;
            attackSubTargetCounter++;
        }
        else if (ini.is_value("attack_preference"))
        {
            std::string shipType = StringUtils::ToLower(std::string(ini.get_value_string(0)));
            uint shipIndex = 0;
            if (shipType == "fighter")
            {
                shipIndex = 0;
            }
            else if (shipType == "freighter")
            {
                shipIndex = 1;
            }
            else if (shipType == "transport")
            {
                shipIndex = 2;
            }
            else if (shipType == "gunboat")
            {
                shipIndex = 3;
            }
            else if (shipType == "cruiser")
            {
                shipIndex = 4;
            }
            else if (shipType == "capital")
            {
                shipIndex = 5;
            }
            else if (shipType == "weapons_platform")
            {
                shipIndex = 8;
            }
            else if (shipType == "solar")
            {
                shipIndex = 10;
            }
            else if (shipType == "anything")
            {
                shipIndex = 11;
            }

            int flag = 0;
            std::string flagText = StringUtils::ToLower(std::string(ini.get_value_string(2)));
            if (flagText.find("guns") != std::string::npos)
            {
                flag += 1;
            }
            if (flagText.find("guided") != std::string::npos)
            {
                flag += 2;
            }
            if (flagText.find("unguided") != std::string::npos)
            {
                flag += 4;
            }
            if (flagText.find("torpedo") != std::string::npos)
            {
                flag += 8;
            }

            pub::AI::Personality::JobStruct::Tattack_order& atkOrder = data.attack_order[attackPrefCounter];
            atkOrder.distance = ini.get_value_float(1);
            atkOrder.flag = flag;
            atkOrder.type = static_cast<int>(shipIndex);
            attackPrefCounter++;
        }
    }

    if (!nick.empty())
    {
        job[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadMissile(INI_Reader& ini)
{
    pub::AI::Personality::MissileUseStruct data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("anti_cruise_missile_interval_time"))
        {
            data.anti_cruise_missile_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("anti_cruise_missile_max_distance"))
        {
            data.anti_cruise_missile_max_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("anti_cruise_missile_min_distance"))
        {
            data.anti_cruise_missile_min_distance = ini.get_value_float(0);
        }
        else if (ini.is_value("anti_cruise_missile_pre_fire_delay"))
        {
            data.anti_cruise_missile_pre_fire_delay = ini.get_value_float(0);
        }
        else if (ini.is_value("missile_launch_allow_out_of_range"))
        {
            data.missile_launch_allow_out_of_range = ini.get_value_bool(0);
        }
        else if (ini.is_value("missile_launch_cone_angle"))
        {
            data.missile_launch_cone_angle = ini.get_value_float(0);
        }
        else if (ini.is_value("missile_launch_interval_time"))
        {
            data.missile_launch_interval_time = ini.get_value_float(0);
        }
        else if (ini.is_value("missile_launch_interval_variance_percent"))
        {
            data.missile_launch_interval_variance_percent = ini.get_value_float(0);
        }
        else if (ini.is_value("missile_launch_range"))
        {
            data.missile_launch_range = ini.get_value_float(0);
        }
    }

    if (!nick.empty())
    {
        missile[std::move(nick)] = data;
    }
}

void PersonalityHelper::LoadPilot(INI_Reader& ini)
{
    pub::AI::Personality data;
    std::string nick;

    while (ini.read_value())
    {
        if (ini.is_value("nickname"))
        {
            nick = ini.get_value_string();
        }
        else if (ini.is_value("inherit"))
        {
            auto personality = Get(ini.get_value_string());
            if (personality.has_value())
            {
                auto& value = personality.value();
                data.BuzzHeadTowardUse = value->BuzzHeadTowardUse;
                data.BuzzPassByUse = value->BuzzPassByUse;
                data.CountermeasureUse = value->CountermeasureUse;
                data.DamageReaction = value->DamageReaction;
                data.EngineKillUse = value->EngineKillUse;
                data.EvadeBreakUse = value->EvadeBreakUse;
                data.EvadeDodgeUse = value->EvadeDodgeUse;
                data.FormationUse = value->FormationUse;
                data.GunUse = value->GunUse;
                data.Job = value->Job;
                data.MineUse = value->MineUse;
                data.MissileUse = value->MissileUse;
                data.MissileReaction = value->MissileReaction;
                data.RepairUse = value->RepairUse;
                data.StrafeUse = value->StrafeUse;
                data.TrailUse = value->TrailUse;
            }
        }
        else if (ini.is_value("gun_id"))
        {
            if (const auto entity = gun.find(ini.get_value_string()); entity != gun.end())
            {
                data.GunUse = entity->second;
            }
        }
        else if (ini.is_value("missile_id"))
        {
            if (const auto entity = missile.find(ini.get_value_string()); entity != missile.end())
            {
                data.MissileUse = entity->second;
            }
        }
        else if (ini.is_value("evade_dodge_id"))
        {
            if (const auto entity = evadeDodge.find(ini.get_value_string()); entity != evadeDodge.end())
            {
                data.EvadeDodgeUse = entity->second;
            }
        }
        else if (ini.is_value("evade_break_id"))
        {
            if (const auto entity = evadeBreak.find(ini.get_value_string()); entity != evadeBreak.end())
            {
                data.EvadeBreakUse = entity->second;
            }
        }
        else if (ini.is_value("buzz_head_toward_id"))
        {
            if (const auto entity = buzzHead.find(ini.get_value_string()); entity != buzzHead.end())
            {
                data.BuzzHeadTowardUse = entity->second;
            }
        }
        else if (ini.is_value("buzz_pass_by_id"))
        {
            if (const auto entity = buzzPass.find(ini.get_value_string()); entity != buzzPass.end())
            {
                data.BuzzPassByUse = entity->second;
            }
        }
        else if (ini.is_value("trail_id"))
        {
            if (const auto entity = trail.find(ini.get_value_string()); entity != trail.end())
            {
                data.TrailUse = entity->second;
            }
        }
        else if (ini.is_value("strafe_id"))
        {
            if (const auto entity = strafe.find(ini.get_value_string()); entity != strafe.end())
            {
                data.StrafeUse = entity->second;
            }
        }
        else if (ini.is_value("engine_kill_id"))
        {
            if (const auto entity = engineKill.find(ini.get_value_string()); entity != engineKill.end())
            {
                data.EngineKillUse = entity->second;
            }
        }
        else if (ini.is_value("mine_id"))
        {
            if (const auto entity = mine.find(ini.get_value_string()); entity != mine.end())
            {
                data.MineUse = entity->second;
            }
        }
        else if (ini.is_value("countermeasure_id"))
        {
            if (const auto entity = cm.find(ini.get_value_string()); entity != cm.end())
            {
                data.CountermeasureUse = entity->second;
            }
        }
        else if (ini.is_value("damage_reaction_id"))
        {
            if (const auto entity = damageReaction.find(ini.get_value_string()); entity != damageReaction.end())
            {
                data.DamageReaction = entity->second;
            }
        }
        else if (ini.is_value("missile_reaction_id"))
        {
            if (const auto entity = missileReaction.find(ini.get_value_string()); entity != missileReaction.end())
            {
                data.MissileReaction = entity->second;
            }
        }
        else if (ini.is_value("formation_id"))
        {
            if (const auto entity = formation.find(ini.get_value_string()); entity != formation.end())
            {
                data.FormationUse = entity->second;
            }
        }
        else if (ini.is_value("repair_id"))
        {
            if (const auto entity = repair.find(ini.get_value_string()); entity != repair.end())
            {
                data.RepairUse = entity->second;
            }
        }
        else if (ini.is_value("job_id"))
        {
            if (const auto entity = job.find(ini.get_value_string()); entity != job.end())
            {
                data.Job = entity->second;
            }
        }
    }

    pilots[std::move(nick)] = data;
}

void PersonalityHelper::LoadFile(const std::string& file)
{
    INI_Reader ini;
    if (!ini.open(file.c_str(), false))
    {
        Log("Unable to parse pilot_population");
        return;
    }

    while (ini.read_header())
    {
        if (ini.is_header("EvadeDodgeBlock"))
        {
            LoadEvadeDodge(ini);
        }
        else if (ini.is_header("EvadeBreakBlock"))
        {
            LoadEvadeBreak(ini);
        }
        else if (ini.is_header("BuzzHeadTowardBlock"))
        {
            LoadBuzzHead(ini);
        }
        else if (ini.is_header("BuzzPassByBlock"))
        {
            LoadBuzzPass(ini);
        }
        else if (ini.is_header("TrailBlock"))
        {
            LoadTrail(ini);
        }
        else if (ini.is_header("StrafeBlock"))
        {
            LoadStrafe(ini);
        }
        else if (ini.is_header("EngineKillBlock"))
        {
            LoadEngineKill(ini);
        }
        else if (ini.is_header("RepairBlock"))
        {
            LoadRepair(ini);
        }
        else if (ini.is_header("GunBlock"))
        {
            LoadGun(ini);
        }
        else if (ini.is_header("MineBlock"))
        {
            LoadMine(ini);
        }
        else if (ini.is_header("MissileBlock"))
        {
            LoadMissile(ini);
        }
        else if (ini.is_header("DamageReactionBlock"))
        {
            LoadDamageReaction(ini);
        }
        else if (ini.is_header("MissileReactionBlock"))
        {
            LoadMissileReaction(ini);
        }
        else if (ini.is_header("CountermeasureBlock"))
        {
            LoadCM(ini);
        }
        else if (ini.is_header("FormationBlock"))
        {
            LoadFormation(ini);
        }
        else if (ini.is_header("JobBlock"))
        {
            LoadJob(ini);
        }
        else if (ini.is_header("Pilot"))
        {
            LoadPilot(ini);
        }
    }
}

PersonalityHelper::PersonalityHelper()
{
    static std::array<std::string, 2> dirs = { "../DATA/MISSIONS/", "../DATA/CHAOS" };
    for (auto dir : dirs)
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
        {
            std::string path = entry.path().generic_string();
            if (!entry.is_regular_file() || entry.file_size() > UINT_MAX ||
                (path.find("npcships") == std::string::npos && path.find("pilots_") == std::string::npos))
            {
                continue;
            }

            LoadFile(path);
        }
    }

    Log(std::format("Parsed Pilot Population: {} personalities", pilots.size()));
}
