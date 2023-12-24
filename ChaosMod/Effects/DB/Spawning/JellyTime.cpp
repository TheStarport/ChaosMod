#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class JellyTime final : public ActiveEffect
{
        std::vector<ResourcePtr<SpawnedObject>> npcs;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            for (auto npc : npcs)
            {
                if (npc.Acquire())
                {
                    SpaceObjectSpawner::i()->Despawn(npc);
                }
            }
            npcs.clear();

            for (auto i = 0u; i < 5; i++)
            {
                if (auto npc = SpaceObjectSpawner::NewBuilder()
                                   .WithNpc("fc_n_no_fighter_d19")
                                   .WithSystem(ship->system)
                                   .WithPosition(ship->get_position(), 250.f)
                                   .WithLevel(25)
                                   .WithReputation("fc_n_grp")
                                   .WithName(237039)
                                   .Spawn();
                    npc.Acquire())
                {
                    npcs.emplace_back(npc);
                }
            }

            for (auto i = 0u; i < 2; i++)
            {
                if (auto npc = SpaceObjectSpawner::NewBuilder()
                                   .WithNpc("MSN12_Nomad_Gunboat")
                                   .WithSystem(ship->system)
                                   .WithPosition(ship->get_position(), 250.f)
                                   .WithLevel(25)
                                   .WithReputation("fc_n_grp")
                                   .WithName(237040)
                                   .Spawn();
                    npc.Acquire())
                {
                    npcs.emplace_back(npc);
                }
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit JellyTime(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(JellyTime, {
    .effectName = "Jelly Time",
    .description = "The Nomads have arrived and they are here to party. But beware, they pack a punch (fruity!)!",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});