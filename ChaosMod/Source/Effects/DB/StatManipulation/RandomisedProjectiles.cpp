#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class RandomisedProjectiles final : public ActiveEffect
{
        inline static std::vector<std::pair<uint, uint>> gunsToOriginalProjectiles;
        using CeGunFire = FireResult(__fastcall*)(CEGun* gun, void* edx, Vector& vec);
        inline static FunctionDetour<CeGunFire> detour{ reinterpret_cast<CeGunFire>(0x62976E0) };
        inline static std::vector<uint> possibleProjectiles;

        static void SwapAmmo(CEquip* equip)
        {
            auto arch = equip->archetype;

            if (std::ranges::find_if(gunsToOriginalProjectiles, [arch](auto& pair) { return pair.first == arch->archId; }) == gunsToOriginalProjectiles.end())
            {
                gunsToOriginalProjectiles.emplace_back(arch->archId, dynamic_cast<Archetype::Launcher*>(arch)->projectileArchId);
            }

            const uint hash = possibleProjectiles[Get<Random>()->Uniform(0u, possibleProjectiles.size() - 1)];

            *reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(arch) + 0x98) = hash;
            *reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(equip) + 0x4C) = reinterpret_cast<DWORD>(Archetype::GetEquipment(hash));
        }

        static FireResult __fastcall Detour(CEGun* gun, void* edx, Vector& vec)
        {
            SwapAmmo(gun);

            detour.UnDetour();
            const auto result = detour.GetOriginalFunc()(gun, edx, vec);
            detour.Detour(Detour);
            return result;
        }

        static void Reset()
        {
            for (const auto& [launcherArch, projectileArch] : gunsToOriginalProjectiles)
            {
                const auto arch = Archetype::GetEquipment(launcherArch);
                if (!arch)
                {
                    continue;
                }

                *reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(arch) + 0x98) = projectileArch;
            }

            gunsToOriginalProjectiles.clear();
        }

        void Begin() override
        {
            Reset();
            detour.Detour(Detour);
        }

        void End() override
        {
            Reset();
            detour.UnDetour();
        }

    public:
        explicit RandomisedProjectiles(const EffectInfo& info) : ActiveEffect(info)
        {
            INI_Reader ini;
            ini.open("freelancer.ini", false);

            std::vector<std::string> files;

            if (!ini.find_header("Data"))
            {
                return;
            }

            while (ini.read_value())
            {
                if (ini.is_value("equipment"))
                {
                    files.emplace_back(ini.get_value_string());
                }
            }

            ini.close();
            for (auto& file : files)
            {
                ini.open(std::format("../DATA/{}", file).c_str(), false);
                while (ini.read_header())
                {
                    if (!ini.is_header("Munition"))
                    {
                        continue;
                    }

                    while (ini.read_value())
                    {
                        if (ini.is_value("nickname"))
                        {
                            possibleProjectiles.emplace_back(CreateID(ini.get_value_string()));
                            break;
                        }
                    }
                }

                ini.close();
            }
        }
};

// clang-format off
SetupEffect(RandomisedProjectiles, {
    .effectName = "Randomised Projectiles",
    .description = "Imagine every shot of your gun was a shot from a different weapon. This effect removes the imagination requirement.",
    .category = EffectType::StatManipulation,
    .timingModifier = 1.0f
});