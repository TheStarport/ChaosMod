#pragma once
#include "Effects/ActiveEffect.hpp"

class CaveatEmptor final : public ActiveEffect
{
        typedef void(__fastcall* DestroySubObjects)(CEqObj* obj, void* edx, DamageList* list, bool b);
        DestroySubObjects destroySubObjects = reinterpret_cast<DestroySubObjects>(0x62ABA50);

        void Begin() override
        {
            CShip* ship = Utils::GetCShip();

            DamageList dmg;
            for (ushort i = 4; i < 34; i++)
            {
                dmg.add_damage_entry(i, 0.0f, static_cast<DamageEntry::SubObjFate>(2));
            }

            destroySubObjects(ship, nullptr, &dmg, true);
        }

    public:
        DefEffectInfo("Caveat Emptor", 0.0f, EffectType::Misc);
};
