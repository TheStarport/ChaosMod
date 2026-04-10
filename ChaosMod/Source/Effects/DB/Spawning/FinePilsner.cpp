
#include "Components/SpaceObjectSpawner.hpp"
#include "Effects/ActiveEffect.hpp"
class FinePilsner final : public ActiveEffect
{
        ResourcePtr<SpawnedObject> vc;
        void Spawn()
        {
            const auto ship = Fluf::GetClient()->GetPlayerCShip();

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

                // TODO: Utils::CatchupNpc(vc, catchingUp);
            }
        }

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