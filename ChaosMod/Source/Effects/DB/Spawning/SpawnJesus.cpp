#include "PCH.hpp"

#include "Components/SpaceObjectSpawner.hpp"
#include "Effects/ActiveEffect.hpp"

class SpawnJesus final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> jesus;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            if (jesus.Acquire())
            {
                Get<SpaceObjectSpawner>()->Despawn(jesus);
            }

            jesus = SpaceObjectSpawner::NewBuilder()
                        .WithArchetype("chaos_jesus")
                        .WithLoadout("chaos_jesus")
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("gun_jesus")
                        .WithReputation("chaos_jesus")
                        .WithName(458757)
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

                Utils::CatchupNpc(jesus, catchingUp, 4000.f);
            }
        }

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