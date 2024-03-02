#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class TakeABreak final : public ActiveEffect
{
        void Begin() override { Teleporter::i()->SaveCurrentPosition(); }

        void End() override { Teleporter::i()->RestorePreviousPosition(); }

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