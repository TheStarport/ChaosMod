// ReSharper disable IdentifierTypo
// ReSharper disable CppInconsistentNaming
#include "PCH.hpp"

#include "Memory/OnSound.hpp"

#include "CoreComponents/ChaosTimer.hpp"

uint* exteriorSoundName = reinterpret_cast<uint*>(GetProcAddress(GetModuleHandleA("common.dll"), "?EXTERIOR_SOUND_NAME@ThrusterEquipConsts@@3UID_String@@A"));
uint* interiorSoundName = reinterpret_cast<uint*>(GetProcAddress(GetModuleHandleA("common.dll"), "?INTERIOR_SOUND_NAME@ThrusterEquipConsts@@3UID_String@@A"));

CEquip* __stdcall OnSound::DetourThrusterFind(CEquipManager* equipManager, EquipmentClass thrusterClass)
{
    originalThrusterSounds = { *interiorSoundName, *exteriorSoundName };

    if (const auto equip = equipManager->FindFirst(thrusterClass))
    {
        if (const auto customSound = customThrusterSoundMap.find(equip->archetype->archId); customSound != customThrusterSoundMap.end())
        {
            *interiorSoundName = customSound->second.first;
            *exteriorSoundName = customSound->second.second;
        }

        return equip;
    }

    return nullptr;
}

__declspec(naked) void OnSound::DetourThrusterFindNaked()
{
    static constexpr DWORD addr = 0x545531;
    __asm {
        push ecx // preserve
        push [esp+4]
        push ecx
        call DetourThrusterFind
        pop ecx
        add esp, 4
        jmp addr
    }
}

CShip* __fastcall OnSound::ShipConstructorDetour(CShip* ship, void* edx, int unk)
{
    shipConstructorDetour.UnDetour();
    const auto result = shipConstructorDetour.GetOriginalFunc()(ship, edx, unk);
    shipConstructorDetour.Detour(ShipConstructorDetour);

    // Reset
    *interiorSoundName = originalThrusterSounds.first;
    *exteriorSoundName = originalThrusterSounds.second;

    return result;
}

bool __fastcall OnSound::SoundIntercept(FlSound* sound)
{
    if (const uint newHash = ChaosTimer::OnSoundEffect(sound->hash); newHash != sound->hash)
    {
        if (!newHash)
        {
            return false;
        }

        pub::Audio::PlaySoundEffect(1, newHash);
        return false;
    }

    detourInterceptSound->UnDetour();
    const auto val = interceptSound(sound);
    detourInterceptSound->Detour(SoundIntercept);
    return val;
}

void OnSound::Init()
{
    detourInterceptSound = std::make_unique<FunctionDetour<InterceptSoundType>>(interceptSound);
    detourInterceptSound->Detour(SoundIntercept);
    shipConstructorDetour.Detour(ShipConstructorDetour);

    MemUtils::PatchAssembly(reinterpret_cast<DWORD>(GetModuleHandleA(nullptr)) + 0x14552B, DetourThrusterFindNaked);

    INI_Reader ini;
    ini.open("freelancer.ini", false);
    ini.find_header("DATA");

    std::list<std::string> files;
    while (ini.read_value())
    {
        if (ini.is_value("equipment"))
        {
            files.emplace_back(ini.get_value_string());
        }
    }

    ini.close();

    std::string path = "../DATA/";
    for (auto& file : files)
    {
        if (!ini.open((path + file).c_str(), false))
        {
            continue;
        }

        while (ini.read_header())
        {
            if (!ini.is_header("thruster"))
            {
                continue;
            }

            uint nickname = 0;
            uint soundInterior = 0;
            uint soundExterior = 0;
            while (ini.read_value())
            {
                if (ini.is_value("nickname"))
                {
                    nickname = CreateID(ini.get_value_string());
                }
                else if (ini.is_value("sound_interior"))
                {
                    soundInterior = CreateID(ini.get_value_string());
                }
                else if (ini.is_value("sound_exterior"))
                {
                    soundExterior = CreateID(ini.get_value_string());
                }
            }

            if (nickname && soundInterior && soundExterior)
            {
                customThrusterSoundMap[nickname] = { soundInterior, soundExterior };
            }
        }

        ini.close();
    }
}
