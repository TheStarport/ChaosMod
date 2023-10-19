#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class SpawnBigBertha final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> bertha;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (bertha.Acquire())
            {
                SpaceObjectSpawner::i()->Despawn(bertha);
            }

            bertha = SpaceObjectSpawner::NewBuilder()
                         .WithArchetype("chaos_big_bertha")
                         .WithLoadout("chaos_big_bertha")
                         .WithSystem(ship->system)
                         .WithPosition(ship->get_position(), 500.f)
                         .WithLevel(50)
                         .WithPersonality("gunboat_default")
                         .WithReputation("gd_gm_grp")
                         .WithName(458754)
                         .Spawn();

            if (bertha.Acquire())
            {
                pub::AI::DirectiveFollowOp op;
                op.followSpaceObj = ship->id;
                op.maxDistance = 0.0f;
                pub::AI::SubmitDirective(bertha.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        RelativeEffectInfo("Spawn Big Bertha", EffectType::Spawning, 2.0f);
};
