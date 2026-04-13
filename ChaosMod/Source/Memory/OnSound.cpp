// ReSharper disable IdentifierTypo
// ReSharper disable CppInconsistentNaming

#include "Memory/OnSound.hpp"

#include "CoreComponents/ChaosTimer.hpp"
#include "FLCore/FLCoreServer.h"

using CreateSoundType = FlSound* (*)(uint& hash);
using PlaySoundType = bool (*)(FlSound* createdSound, int, int);
using InterceptSoundType = bool(__fastcall*)(FlSound*);
using ShipConstructorType = CShip*(__fastcall*)(CShip* ship, void* edx, int unk);

std::pair<uint, uint> originalThrusterSounds;
std::unordered_map<uint, std::pair<uint, uint>> customThrusterSoundMap;
FunctionDetour shipConstructorDetour{ reinterpret_cast<ShipConstructorType>(reinterpret_cast<DWORD>(GetModuleHandleA(nullptr)) + 0x12EFB0) };
inline static auto createSound = reinterpret_cast<CreateSoundType>(0x42ae40);
inline static auto playSound = reinterpret_cast<PlaySoundType>(0x4285f0);
inline static auto interceptSound = reinterpret_cast<InterceptSoundType>(0x42B840);
// inline static std::unique_ptr<FunctionDetour<PlaySoundType>> detourPlaySound = nullptr;
inline static std::unique_ptr<FunctionDetour<InterceptSoundType>> detourInterceptSound = nullptr;

static bool __fastcall SoundIntercept(FlSound* sound);

auto exteriorSoundName = reinterpret_cast<uint*>(GetProcAddress(GetModuleHandleA("common.dll"), "?EXTERIOR_SOUND_NAME@ThrusterEquipConsts@@3UID_String@@A"));
auto interiorSoundName = reinterpret_cast<uint*>(GetProcAddress(GetModuleHandleA("common.dll"), "?INTERIOR_SOUND_NAME@ThrusterEquipConsts@@3UID_String@@A"));

CEquip* __stdcall DetourThrusterFind(CEquipManager* equipManager, EquipmentClass thrusterClass)
{
    originalThrusterSounds = { *interiorSoundName, *exteriorSoundName };

    if (const auto equip = equipManager->FindFirst(static_cast<int>(thrusterClass)))
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

constexpr DWORD returnAddr = 0x545531;
__declspec(naked) void DetourThrusterFindNaked()
{
    __asm {
        push ecx // preserve
        push [esp+4]
        push ecx
        call DetourThrusterFind
        pop ecx
        add esp, 4
        jmp returnAddr
    }
}

CShip* __fastcall ShipConstructorDetour(CShip* ship, void* edx, int unk)
{
    shipConstructorDetour.UnDetour();
    const auto result = shipConstructorDetour.GetOriginalFunc()(ship, edx, unk);
    shipConstructorDetour.Detour(ShipConstructorDetour);

    // Reset
    *interiorSoundName = originalThrusterSounds.first;
    *exteriorSoundName = originalThrusterSounds.second;

    return result;
}

bool __fastcall SoundIntercept(FlSound* sound)
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
