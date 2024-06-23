#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/SpaceObjectSpawner.hpp"

class SpawnEvilPlayer final : public ActiveEffect
{
        int i = 0;
        float correctionTimer = 1.f;
        bool catchingUp = false;

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

                SpaceObjectSpawner::LoadoutItem item{ nickname.value(), desc.is_equipped() ? 0u : desc.count, desc.is_internal() ? "" : desc.hardPoint.value };

                items.emplace_back(item);
            }

            items.emplace_back("infinite_power", 0, "");

            const std::string nickname = "cloned_player_loadout_evil_" + std::to_string(i++);
            SpaceObjectSpawner::CreateNewLoadout(nickname, items);

            clone = SpaceObjectSpawner::NewBuilder()
                        .WithArchetype(ship->archetype->archId)
                        .WithLoadout(nickname)
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("pilot_universal")
                        .WithReputation("fc_uk_grp")
                        .WithName(458858)
                        .WithFuse("chaos_teleport_fx")
                        .Spawn();

            if (const auto npc = clone.Acquire(); npc)
            {
                int reputation;
                pub::Player::GetRep(1, reputation);
                int npcReputation;
                pub::SpaceObj::GetRep(npc->spaceObj, npcReputation);
                pub::Reputation::SetAttitude(npcReputation, reputation, -0.9f);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

        void Update(const float delta) override
        {
            correctionTimer -= delta;
            if (correctionTimer < 0.f)
            {
                correctionTimer = 1.f;

                Utils::CatchupNpc(clone, catchingUp);
            }
        }

    public:
        explicit SpawnEvilPlayer(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SpawnEvilPlayer, {
    .effectName = "Spawn Evil Player",
    .description = "",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});