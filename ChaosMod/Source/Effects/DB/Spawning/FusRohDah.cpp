#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/SpaceObjectSpawner.hpp"

class FusRohDah final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> dragon;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (dragon.Acquire())
            {
                Get<SpaceObjectSpawner>()->Despawn(dragon);
            }

            dragon = SpaceObjectSpawner::NewBuilder()
                        .WithArchetype("chaos_dragon")
                        .WithLoadout("chaos_dragon")
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("pilot_dragon")
                        .WithReputation("chaos_hostile")
                        .WithName(458865)
                        .WithFuse("chaos_teleport_fx")
                        .Spawn();

            if (dragon.Acquire())
            {
                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.f;
                op.x14 = true;
                op.fireWeapons = true;
                pub::AI::SubmitDirective(dragon.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit FusRohDah(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FusRohDah, {
    .effectName = "FusRohDah",
    .description = "",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});