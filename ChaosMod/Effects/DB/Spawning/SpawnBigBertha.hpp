#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class SpawnBigBertha final : public ActiveEffect
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
                .WithArchetype("chaos_big_bertha")
                .WithLoadout("chaos_big_bertha")
                .WithSystem(ship->system)
                .WithPosition(ship->get_position(), 500.f)
                .WithLevel(50)
                .WithPersonality("gunboat_default")
                .WithReputation("gd_gm_grp")
                .WithName(458754)
                .Spawn();

            if (jesus.Acquire())
            {
                pub::AI::DirectiveFollowOp op;
                op.followSpaceObj = ship->id;
                op.maxDistance = 0.0f;
                pub::AI::SubmitDirective(jesus.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        DefEffectInfo("Spawn Big Bertha", 2.0f, EffectType::Spawning);
};
