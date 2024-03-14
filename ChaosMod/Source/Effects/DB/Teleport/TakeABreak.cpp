#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/Teleporter.hpp"

class TakeABreak final : public ActiveEffect
{
        void Begin() override { Get<Teleporter>()->SaveCurrentPosition(); }

        void End() override { Get<Teleporter>()->RestorePreviousPosition(); }

    public:
        explicit TakeABreak(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(TakeABreak, {
    .effectName = "Take a break",
    .description = "You've been playing too long. We've captured your progress and set a manditory break time.",
    .category = EffectType::Teleport,
    .timingModifier = 1.0f
});