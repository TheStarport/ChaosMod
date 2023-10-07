#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class SpawnJesus final : public ActiveEffect
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
                        .WithArchetype("chaos_jesus")
                        .WithLoadout("chaos_jesus")
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("pilot_military_ace")
                        .WithReputation("chaos_jesus")
                        .WithName(458757)
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
        DefEffectInfo("Spawn Griefer Jesus", 2.0f, EffectType::Spawning);
};
