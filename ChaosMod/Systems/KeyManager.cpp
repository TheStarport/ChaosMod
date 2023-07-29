#include "PCH.hpp"

#include "KeyManager.hpp"

bool __stdcall KeyManager::HandleKey(int keyCmd)
{
    // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
    switch (keyCmd)
    {
        case Utils::USER_NONE:
        case Utils::USER_KEY:
        case Utils::USER_CHAR:
        case Utils::USER_BUTTON:
        case Utils::USER_WHEEL:
        case Utils::USER_MOVE:
        case Utils::USER_BUTTON0:
        case Utils::USER_BUTTON1:
        case Utils::USER_BUTTON2:
        case Utils::USER_BUTTON3:
        case Utils::USER_BUTTON4:
        case Utils::USER_BUTTON5:
        case Utils::USER_BUTTON6:
        case Utils::USER_BUTTON7:
        case Utils::USER_EVENTS:
        case Utils::USER_CANCEL:
        case Utils::USER_STACKABLE_CANCEL:
        case Utils::USER_ENTER:
        case Utils::USER_NO:
        case Utils::USER_YES:
        case Utils::USER_RESET: return false;

        default: break;
    }

    if (const auto instance = i(); instance->randomiseKeys)
    {
        if (nextKeyRandom)
        {
            nextKeyRandom = false;
            return false;
        }
        else
        {
            nextKeyRandom = true;
            using HandleKeyType = bool (*)(int, int);
            static auto handleKey = reinterpret_cast<HandleKeyType>(0x576410);
            const auto key = instance->keyMap[keyCmd];
            handleKey(key, key);
            return true;
        }
    }

    switch (keyCmd) // NOLINT(hicpp-multiway-paths-covered)
    {
        default: return false;
    }
}

void __declspec(naked) HandleKeyNaked()
{
    __asm
    {
        push[esp + 4]
		call KeyManager::HandleKey
		test al, al
		jz   cmd_not_processed
		ret

		cmd_not_processed :
		    sub esp, 80h
		    push ebx
		    push 0x00576417
		    ret

    }
}

KeyManager::KeyManager()
{
    BYTE patch[] = { 0xB9, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE1 }; // mov ecx HkCb_HandleKeyCmdNaked, jmp ecx
    auto address = reinterpret_cast<PDWORD>(reinterpret_cast<char*>(&patch) + 1);
    *address = reinterpret_cast<DWORD>(PVOID(HandleKeyNaked));
    Utils::Memory::WriteProcMem(reinterpret_cast<char*>(0x576410), &patch, 7);

    // Setup possible values
    possibleKeys = { Utils::USER_PAUSE,
                     Utils::USER_NEXT_TARGET,
                     Utils::USER_PREV_TARGET,
                     Utils::USER_NEXT_SUBTARGET,
                     Utils::USER_PREV_SUBTARGET,
                     Utils::USER_CLEAR_TARGET,
                     Utils::USER_NEXT_ENEMY,
                     Utils::USER_PREV_ENEMY,
                     Utils::USER_CLOSEST_ENEMY,
                     Utils::USER_X_ROTATE,
                     Utils::USER_X_UNROTATE,
                     Utils::USER_Y_ROTATE,
                     Utils::USER_Y_UNROTATE,
                     Utils::USER_Z_ROTATE,
                     Utils::USER_Z_UNROTATE,
                     Utils::USER_COCKPIT_CAMERA_MODE,
                     Utils::USER_LOOK_ROTATE_CAMERA_LEFT,
                     Utils::USER_LOOK_ROTATE_CAMERA_RIGHT,
                     Utils::USER_LOOK_ROTATE_CAMERA_UP,
                     Utils::USER_LOOK_ROTATE_CAMERA_DOWN,
                     Utils::USER_LOOK_ROTATE_CAMERA_RESET,
                     Utils::USER_REARVIEW_CAMERA_MODE,
                     Utils::USER_REARVIEW_CAMERA_MODE_OFF,
                     Utils::USER_AFTERBURN,
                     Utils::USER_AFTERBURN_OFF,
                     Utils::USER_AFTERBURN_ON,
                     Utils::USER_TOGGLE_AUTO_AVOIDANCE,
                     Utils::USER_TOGGLE_AUTO_LEVEL,
                     Utils::USER_TOGGLE_LEVEL_CAMERA,
                     Utils::USER_INC_THROTTLE,
                     Utils::USER_DEC_THROTTLE,
                     Utils::USER_CRUISE,
                     Utils::USER_CONTACT_LIST,
                     Utils::USER_RADAR_ZOOM,
                     Utils::USER_SWITCH_TO_PLAYER_SHIP,
                     Utils::USER_SWITCH_TO_WEAPON_LIST,
                     Utils::USER_SWITCH_TO_TARGET,
                     Utils::USER_FORMATION_LIST,
                     Utils::USER_RADIO,
                     Utils::USER_CHAT,
                     Utils::USER_STATUS,
                     Utils::USER_TARGET,
                     Utils::USER_NAV_MAP,
                     Utils::USER_PLAYER_STATS,
                     Utils::USER_INVENTORY,
                     Utils::USER_STORY_STAR,
                     Utils::USER_CHAT_WINDOW,
                     Utils::USER_GROUP_WINDOW,
                     Utils::USER_HELP,
                     Utils::USER_INVENTORY_CLOSE,
                     Utils::USER_MINIMIZE_HUD,
                     Utils::USER_DISPLAY_LAST_OBJECTIVE,
                     Utils::USER_COLLECT_LOOT,
                     Utils::USER_MANEUVER_TRAIL,
                     Utils::USER_MANEUVER_EVADE,
                     Utils::USER_MANEUVER_ENGINEKILL,
                     Utils::USER_MANEUVER_BRAKE_REVERSE,
                     Utils::USER_MANEUVER_DOCK,
                     Utils::USER_MANEUVER_GOTO,
                     Utils::USER_MANEUVER_FACE,
                     Utils::USER_MANEUVER_DRASTIC_EVADE,
                     Utils::USER_MANEUVER_FORMATION,
                     Utils::USER_MANEUVER_STRAFE,
                     Utils::USER_MANEUVER_TRAIL_CLOSER,
                     Utils::USER_MANEUVER_TRAIL_FARTHER,
                     Utils::USER_MANEUVER_CORKSCREW_EVADE,
                     Utils::USER_MANEUVER_SLIDE_EVADE,
                     Utils::USER_MANEUVER_SLIDE_EVADE_LEFT,
                     Utils::USER_MANEUVER_SLIDE_EVADE_RIGHT,
                     Utils::USER_MANEUVER_SLIDE_EVADE_UP,
                     Utils::USER_MANEUVER_SLIDE_EVADE_DOWN,
                     Utils::USER_SCAN_CARGO,
                     Utils::USER_TRACTOR_BEAM,
                     Utils::USER_REPAIR_HEALTH,
                     Utils::USER_REPAIR_SHIELD,
                     Utils::USER_WEAPON_GROUP1,
                     Utils::USER_WEAPON_GROUP2,
                     Utils::USER_WEAPON_GROUP3,
                     Utils::USER_WEAPON_GROUP4,
                     Utils::USER_WEAPON_GROUP5,
                     Utils::USER_WEAPON_GROUP6,
                     Utils::USER_TOGGLE_WEAPON1,
                     Utils::USER_TOGGLE_WEAPON2,
                     Utils::USER_TOGGLE_WEAPON3,
                     Utils::USER_TOGGLE_WEAPON4,
                     Utils::USER_TOGGLE_WEAPON5,
                     Utils::USER_TOGGLE_WEAPON6,
                     Utils::USER_TOGGLE_WEAPON7,
                     Utils::USER_TOGGLE_WEAPON8,
                     Utils::USER_TOGGLE_WEAPON9,
                     Utils::USER_TOGGLE_WEAPON10,
                     Utils::USER_FIRE_WEAPON1,
                     Utils::USER_FIRE_WEAPON2,
                     Utils::USER_FIRE_WEAPON3,
                     Utils::USER_FIRE_WEAPON4,
                     Utils::USER_FIRE_WEAPON5,
                     Utils::USER_FIRE_WEAPON6,
                     Utils::USER_FIRE_WEAPON7,
                     Utils::USER_FIRE_WEAPON8,
                     Utils::USER_FIRE_WEAPON9,
                     Utils::USER_FIRE_WEAPON10,
                     Utils::USER_FIRE_WEAPON_GROUP1,
                     Utils::USER_FIRE_WEAPON_GROUP2,
                     Utils::USER_FIRE_WEAPON_GROUP3,
                     Utils::USER_FIRE_WEAPON_GROUP4,
                     Utils::USER_FIRE_WEAPON_GROUP5,
                     Utils::USER_FIRE_WEAPON_GROUP6,
                     Utils::USER_ASSIGN_WEAPON_GROUP1,
                     Utils::USER_ASSIGN_WEAPON_GROUP2,
                     Utils::USER_ASSIGN_WEAPON_GROUP3,
                     Utils::USER_ASSIGN_WEAPON_GROUP4,
                     Utils::USER_ASSIGN_WEAPON_GROUP5,
                     Utils::USER_ASSIGN_WEAPON_GROUP6,
                     Utils::USER_REMAPPABLE_LEFT,
                     Utils::USER_REMAPPABLE_RIGHT,
                     Utils::USER_REMAPPABLE_UP,
                     Utils::USER_REMAPPABLE_DOWN,
                     Utils::USER_FIRE_FORWARD,
                     Utils::USER_LAUNCH_MISSILES,
                     Utils::USER_LAUNCH_MINES,
                     Utils::USER_LAUNCH_COUNTERMEASURES,
                     Utils::USER_AUTO_TURRET,
                     Utils::USER_LAUNCH_TORPEDOS,
                     Utils::USER_LAUNCH_CRUISE_DISRUPTORS,
                     Utils::USER_NEXT_OBJECT,
                     Utils::USER_PREV_OBJECT,
                     // Utils::USER_EXIT_GAME,
                     Utils::USER_MANEUVER_FREE_FLIGHT,
                     Utils::USER_FIRE_WEAPONS,
                     Utils::USER_TURN_SHIP,
                     Utils::USER_GROUP_INVITE,
                     Utils::USER_TRADE_REQUEST,
                     Utils::USER_SCREEN_SHOT };

    for (auto& key : possibleKeys)
    {
        keyMap[key] = key;
    }
}

void KeyManager::ToggleRandomisedKeys()
{
    randomiseKeys = !randomiseKeys;
    if (!randomiseKeys)
    {
        return;
    }

    std::vector<int> keys;
    for (auto& key : possibleKeys)
    {
        keys.emplace_back(key);
    }

    static const uint seed = static_cast<uint>(std::chrono::system_clock::now().time_since_epoch().count());
    static std::default_random_engine engine(seed);
    std::ranges::shuffle(keys, engine);

    for (uint i = 0; i < keyMap.size() && i < keys.size(); i++)
    {
        auto it = keyMap.begin();
        std::advance(it, i);

        it->second = keys[i];
    }
}
