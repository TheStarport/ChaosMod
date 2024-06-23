#include "PCH.hpp"

#include "Components/SpaceObjectSpawner.hpp"
#include "Effects/ActiveEffect.hpp"

class SpawnExtremeJesus final : public ActiveEffect
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
                        .WithArchetype("chaos_jesus_extreme")
                        .WithLoadout("chaos_jesus_extreme")
                        .WithSystem(ship->system)
                        .WithPosition(ship->get_position(), 1500.f)
                        .WithLevel(50)
                        .WithPersonality("gun_jesus")
                        .WithReputation("chaos_jesus")
                        .WithFuse("chaos_teleport_fx")
                        .WithName(458758)
                        .Spawn();
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

        void OnSystemUnload() override
        {
            if (jesus.Acquire())
            {
                Get<SpaceObjectSpawner>()->Despawn(jesus);
            }
        }

        void OnJumpInComplete() override { Spawn(); }

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
        explicit SpawnExtremeJesus(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SpawnExtremeJesus, {
    .effectName = "Spawn Extreme Griefer Jesus",
    .description = "",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});