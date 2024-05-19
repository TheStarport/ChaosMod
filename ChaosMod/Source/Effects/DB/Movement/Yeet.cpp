#include "Components/ShipManipulator.hpp"
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Yeet final : public ActiveEffect
{
        Vector movement{};
        void Begin() override
        {
            movement.x = Get<Random>()->Uniform(0, 1) ? -3000.f : 3000.f;
            movement.y = Get<Random>()->Uniform(0, 1) ? -3000.f : 3000.f;
            movement.z = Get<Random>()->Uniform(0, 1) ? -3000.f : 3000.f;
        }

        // Ensure they cannot restart their engine for 5% of effect duration
        void Update(float delta) override
        {
            const auto ship = Utils::GetCShip();
            ship->get_behavior_interface()->update_current_behavior_engage_engine(false);
            ShipManipulator::SetVelocity(ship, movement);
        }

    public:
        explicit Yeet(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Yeet, {
    .effectName = "YEET",
    .description = "God decided you are to be yeeted away at high speed.",
    .category = EffectType::Movement,
    .fixedTimeOverride = 4.0f
});