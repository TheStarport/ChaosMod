#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/SpaceObjectSpawner.hpp"

class Pastafarianism final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> spaghetti;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (spaghetti.Acquire())
            {
                Get<SpaceObjectSpawner>()->Despawn(spaghetti);
            }

            spaghetti = SpaceObjectSpawner::NewBuilder()
                        .WithArchetype("chaos_spaghetti")
                        .WithLoadout("chaos_spaghetti")
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("gun_jesus")
                        .WithReputation("chaos_pasta_church")
                        .WithName(458864)
                        .WithFuse("chaos_teleport_fx")
                        .Spawn();

            if (spaghetti.Acquire())
            {
                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.f;
                op.x14 = true;
                op.fireWeapons = true;
                pub::AI::SubmitDirective(spaghetti.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit Pastafarianism(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Pastafarianism, {
    .effectName = "Pastafarianism",
    .description = "He boiled for your sins",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});