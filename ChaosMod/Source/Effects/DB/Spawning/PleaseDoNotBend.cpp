#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/SpaceObjectSpawner.hpp"

class PleaseDoNotBend final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> maxwell;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (maxwell.Acquire())
            {
                Get<SpaceObjectSpawner>()->Despawn(maxwell);
            }

            maxwell = SpaceObjectSpawner::NewBuilder()
                          .WithArchetype("chaos_maxwell_space")
                          .WithLoadout("chaos_maxwell")
                          .WithSystem(ship->system)
                          .WithPosition(ship->get_position(), 500.f)
                          .WithLevel(50)
                          .WithPersonality("gun_jesus")
                          .WithReputation("fc_uk_grp")
                          .WithName(458856)
                          .WithFuse("chaos_teleport_fx")
                          .Spawn();

            if (maxwell.Acquire())
            {
                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.f;
                op.x14 = true;
                op.fireWeapons = true;
                pub::AI::SubmitDirective(maxwell.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit PleaseDoNotBend(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PleaseDoNotBend, {
    .effectName = "Please Do Not Bend",
    .description = "Freelancer, but I have a cat.",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});