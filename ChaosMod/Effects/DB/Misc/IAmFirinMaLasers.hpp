#pragma once
#include "Effects/ActiveEffect.hpp"

class IAmFiringMyLasers final : public ActiveEffect
{
        typedef void*(__fastcall* UnkClassType)(void* ptr, void* edx, CELauncher* launcher, void* unk);
        inline static std::unique_ptr<FunctionDetour<UnkClassType>> detour;
        inline static std::map<CELauncher*, void*> weirdFreelancerClassMap;

        static void* __fastcall Detour(void* ptr, void* edx, CELauncher* launcher, void* unk)
        {
            detour->UnDetour();
            const auto result = detour->GetOriginalFunc()(ptr, edx, launcher, unk);
            detour->Detour(Detour);
            weirdFreelancerClassMap[launcher] = result;
            return result;
        }

        void Update(float delta) override
        {
            if (CShip* ship = Utils::GetCShip())
            {
                CEquipTraverser tr;
                CEquip* equip = GetEquipManager(ship)->Traverse(tr);
                while (equip)
                {
                    EquipDesc e;
                    equip->GetEquipDesc(e);
                    if (CEGun::cast(equip))
                    {
                        const auto launcher = CEGun::cast(equip);

                        const auto range = launcher->GetMunitionRange();
                        Vector target = launcher->GetBarrelDirWS(0);
                        const auto [x, y, z] = launcher->GetBarrelPosWS(0);
                        target.x *= range;
                        target.x += x;
                        target.y *= range;
                        target.y += y;
                        target.z *= range;
                        target.z += z;

                        if (!launcher->RefireDelayElapsed() && launcher->CanFire(target))
                        {
                            equip = GetEquipManager(ship)->Traverse(tr);
                            continue;
                        }

                        void* randClass = weirdFreelancerClassMap[launcher];

                        auto ar = launcher->ProjectileArch();

                        typedef DWORD(__fastcall * FireFunctionType)(void* thing, void* edx, Vector& vec);
                        const auto fireFunc = static_cast<FireFunctionType>((*static_cast<void***>(randClass))[3]);
                        fireFunc(randClass, nullptr, target);

                        if (const auto fireSfx = reinterpret_cast<PDWORD>(*reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(ar) + 0x4) + 0x5C))
                        {
                            pub::Audio::PlaySoundEffect(1, *fireSfx);
                        }
                    }

                    equip = GetEquipManager(ship)->Traverse(tr);
                }
            }
        }

    public:
        IAmFiringMyLasers()
        {
            detour = std::make_unique<FunctionDetour<UnkClassType>>(reinterpret_cast<UnkClassType>(0x52D880));
            detour->Detour(Detour);
            weirdFreelancerClassMap.clear();
        }

        DefEffectInfo("I'm Firin Ma Lasers", 1.0f, EffectType::Misc);
};
