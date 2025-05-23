#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ShipManipulator.hpp"

class IllTrySpinning final : public ActiveEffect
{
        void Begin() override
        {
            Get<ShipManipulator>()->MakeShipsSpin(true);
            Utils::GetCShip()->get_behavior_interface()->update_current_behavior_auto_level(false);
        }

        void End() override
        {
            Get<ShipManipulator>()->MakeShipsSpin(false);
            auto ship = Utils::GetCShip();
            if (ship)
            {
                ship->get_behavior_interface()->update_current_behavior_auto_level(true);
            }
        }

    public:
        explicit IllTrySpinning(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(IllTrySpinning, {
    .effectName = "I'll Try Spinning",
    .description = "All ships (including yours) go for a spin!",
    .category = EffectType::Movement
});
