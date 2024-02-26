#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/SystemComponents/GlobalTimers.hpp"

class CaveatEmptor final : public ActiveEffect
{
        // TODO: Have destroyed equipment persist after dock
        typedef void(__fastcall* DestroySubObjects)(CEqObj* obj, void* edx, DamageList* list, bool b);
        inline static DestroySubObjects destroySubObjects = reinterpret_cast<DestroySubObjects>(0x62ABA50);

        static bool DelayedGroupExplosion(float delta)
        {
            CShip* ship = Utils::GetCShip();
            if (!ship)
            {
                return true;
            }

            DamageList dmg;
            const Archetype::CollisionGroup* group = ship->shiparch()->collisionGroup;
            while (group)
            {
                dmg.add_damage_entry(group->id, 0.0f, static_cast<DamageEntry::SubObjFate>(2));
                group = group->next;
            }

            destroySubObjects(ship, nullptr, &dmg, false);
            return true;
        }

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_caveat_emptor"));

            // Blow off guns that are not attached to the ship root
            CShip* ship = Utils::GetCShip();

            std::vector<CAttachedEquip*> attachedEquips;

            CEquipTraverser tr;
            CEquip* equip = ship->equip_manager.Traverse(tr);
            while (equip)
            {
                auto external = CAttachedEquip::cast(equip);
                equip = ship->equip_manager.Traverse(tr);

                if (!external || external->GetParentConnector(true) == long(ship->index))
                {
                    continue;
                }

                // We are attached to the ship
                attachedEquips.emplace_back(external);
            }

            DamageList dmg;
            for (const auto subEquip : attachedEquips)
            {
                // Apply damage to destroy equipment
                dmg.add_damage_entry(subEquip->SubObjId, 0.0f, static_cast<DamageEntry::SubObjFate>(2));
            }

            destroySubObjects(ship, nullptr, &dmg, false);

            // Queue wing destruction
            GlobalTimers::i()->AddTimer(DelayedGroupExplosion, 1.0f);
        }

    public:
        explicit CaveatEmptor(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(CaveatEmptor, {
    .effectName = "Caveat Emptor",
    .description = "Caveat Emptor, buyer beware. Sounds like that Liberty Defender you bought had a few issues with it, and as a result all the wings fell off!",
    .category = EffectType::Misc,
    .timingModifier = 0.0f,
    .isTimed = false
});