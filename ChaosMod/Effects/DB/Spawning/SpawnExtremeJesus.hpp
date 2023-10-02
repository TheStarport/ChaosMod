#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class SpawnExtremeJesus final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> jesus;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (jesus.Acquire())
            {
                SpaceObjectSpawner::i()->Despawn(jesus);
            }

            jesus = SpaceObjectSpawner::NewBuilder()
                        .WithArchetype("chaos_jesus_extreme")
                        .WithLoadout("chaos_jesus_extreme")
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("gun_military_ace_style_a")
                        .WithReputation("chaos_jesus")
                        .WithName(458758)
                        .Spawn();

            if (jesus.Acquire())
            {
                pub::AI::SetZoneBehaviorParams op;
                op.range = 25000.f;
                op.spaceObj = ship->id;
                op.zoneType = 2;
                pub::AI::SubmitDirective(jesus.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        DefEffectInfo("Spawn Extreme Greifer Jesus", 2.0f, EffectType::Spawning);
};
