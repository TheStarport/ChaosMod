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
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

        float correctionTimer = 1.0f;
        bool catchingUp = false;
        void Update(const float delta) override
        {
            correctionTimer -= delta;
            if (correctionTimer < 0.f)
            {
                correctionTimer = 1.f;

                Utils::CatchupNpc(spaghetti, catchingUp, 4000.f);
            }
        }

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