#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ShipManipulator.hpp"

class EnableGravity final : public ActiveEffect
{
        void Begin() override
        {
            Get<ShipManipulator>()->EnableGravity(true);
        }

        void End() override
        {
            Get<ShipManipulator>()->EnableGravity(false);
        }

    public:
        explicit EnableGravity(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(EnableGravity, {
    .effectName = "Enable Gravity",
    .description = "The fact this game has no gravity is so unrealistic.",
    .category = EffectType::Movement
});
