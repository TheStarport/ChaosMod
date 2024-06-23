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
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }
        void OnSystemUnload() override
        {
            auto ship = dragon.Acquire();
            if (ship)
            {
                ((CShip*)ship->obj)->flush_animations();
                Get<SpaceObjectSpawner>()->Despawn(dragon);
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

                Utils::CatchupNpc(dragon, catchingUp, 6000.f);
            }
        }

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