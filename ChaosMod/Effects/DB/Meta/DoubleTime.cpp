#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ChaosTimer.hpp"

class DoubleTime final : public ActiveEffect
{
        void Begin() override { ChaosTimer::i()->ToggleDoubleTime(); }
        void End() override { ChaosTimer::i()->ToggleDoubleTime(); }

    public:
        explicit DoubleTime(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DoubleTime, {
    .effectName = "Double Time!",
    .description = "Well you've had first effect, but what about second effect?",
    .category = EffectType::Meta,
});