#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class MonsterCloset final : public ActiveEffect
{
        std::vector<ResourcePtr<SpawnedObject>> npcs;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            for (auto npc : npcs)
            {
                if (npc.Acquire())
                {
                    SpaceObjectSpawner::i()->Despawn(npc);
                }
            }

            npcs.clear();

            const uint count = Random::i()->Uniform(5u, 15u);

            for (auto i = 0u; i < count; i++)
            {

                npcs.emplace_back(SpaceObjectSpawner::NewBuilder()
                                      .WithRandomNpc()
                                      .WithRandomReputation()
                                      .WithRandomName()
                                      .WithPosition(ship->get_position(), 400.f)
                                      .WithSystem(ship->system)
                                      .WithFuse("chaos_teleport_fx")
                                      .WithLevel(50) // Stop them from fleeing
                                      .Spawn());
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit MonsterCloset(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(MonsterCloset, {
    .effectName = "Monster Closet",
    .description = "",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});