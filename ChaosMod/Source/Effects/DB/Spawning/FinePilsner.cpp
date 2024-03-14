#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/SpaceObjectSpawner.hpp"

class FinePilsner final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> vc;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (vc.Acquire())
            {
                Get<SpaceObjectSpawner>()->Despawn(vc);
            }

            vc = SpaceObjectSpawner::NewBuilder()
                     .WithNpc("rh_n_rh_elite_d12-19")
                     .WithSystem(ship->system)
                     .WithPosition(ship->get_position(), 500.f)
                     .WithLevel(50)
                     .WithReputation("fc_b_grp")
                     .WithAbsoluteHealth(24000)
                     .WithName(217002)
                     .WithFuse("chaos_teleport_fx")
                     .Spawn();

            if (vc.Acquire())
            {
                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.f;
                op.x14 = true;
                op.fireWeapons = true;
                pub::AI::SubmitDirective(vc.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit FinePilsner(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FinePilsner, {
    .effectName = "Fine Pilsner",
    .description = "A fine companion to assist you in your quest!",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});