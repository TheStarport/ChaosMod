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
                        .WithPersonality("pilot_military_ace")
                        .WithReputation("chaos_jesus")
                        .WithName(458758)
                        .Spawn();

            if (jesus.Acquire())
            {
                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.f;
                pub::AI::SubmitDirective(jesus.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        DefEffectInfo("Spawn Extreme Greifer Jesus", 2.0f, EffectType::Spawning);
};
