#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/SpaceObjectSpawner.hpp"

class ClonePlayer final : public ActiveEffect
{
        int i = 0;
        ResourcePtr<SpawnedObject> clone;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (clone.Acquire())
            {
                Get<SpaceObjectSpawner>()->Despawn(clone);
            }

            std::vector<SpaceObjectSpawner::LoadoutItem> items;
            for (auto traverser = ship->equip_manager.begin(); traverser != ship->equip_manager.end(); ++traverser)
            {
                auto nickname = ChaosMod::HashLookup(traverser.currentEquip->archetype->archId);
                if (!nickname.has_value())
                {
                    continue;
                }

                EquipDesc desc;
                traverser.currentEquip->GetEquipDesc(desc);

                SpaceObjectSpawner::LoadoutItem item{ nickname.value(), desc.is_equipped() ? 0 : desc.count, desc.is_internal() ? "" : desc.hardPoint.value };

                items.emplace_back(item);
            }

            const std::string nickname = "cloned_player_loadout_" + std::to_string(i++);
            SpaceObjectSpawner::CreateNewLoadout(nickname, items);

            clone = SpaceObjectSpawner::NewBuilder()
                        .WithArchetype(ship->archetype->archId)
                        .WithLoadout(nickname)
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("pilot_universal")
                        .WithReputation("fc_uk_grp")
                        .WithName(458857)
                        .WithFuse("chaos_teleport_fx")
                        .Spawn();

            if (clone.Acquire())
            {
                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.f;
                op.x14 = true;
                op.fireWeapons = true;
                pub::AI::SubmitDirective(clone.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit ClonePlayer(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(ClonePlayer, {
    .effectName = "Clone Player",
    .description = "",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});