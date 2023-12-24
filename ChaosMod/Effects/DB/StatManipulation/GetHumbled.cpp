#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class GetHumbled final : public ActiveEffect
{
        void Begin() override
        {
            int cash;
            pub::Player::InspectCash(1, cash);

            const int newCash = cash * 10 / 100;
            pub::Player::AdjustCash(1, -cash + newCash);
        }

    public:
        explicit GetHumbled(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(GetHumbled, {
    .effectName = "Get Humbled",
    .description = "I hope you wern't getting used to having all that money? Because 90% of it is gone now.",
    .category = EffectType::StatManipulation,
    .timingModifier = 0.0f,
    .isTimed = false
});