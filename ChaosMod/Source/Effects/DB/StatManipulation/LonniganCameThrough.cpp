#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class LonniganCameThrough final : public ActiveEffect
{
        void Begin() override { pub::Player::AdjustCash(1, 1'000'000); }

    public:
        explicit LonniganCameThrough(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(LonniganCameThrough, {
    .effectName = "Lonnigan Came Through",
    .description = "You know, I always believed that Trent would get his 1 million credits.",
    .category = EffectType::StatManipulation,
    .timingModifier = 0.0f,
    .isTimed = false
});