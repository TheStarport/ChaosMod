#include "PCH.hpp"

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
                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.f;
                op.x14 = true;
                op.fireWeapons = true;
                pub::AI::SubmitDirective(jesus.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit SpawnJesus(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SpawnJesus, {
    .effectName = "Spawn Griefer Jesus",
    .description = "",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});