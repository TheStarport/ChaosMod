#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ChaosTimer.hpp"

class DoubleTime final : public ActiveEffect
{
        void Begin() override { Get<ChaosTimer>()->ToggleDoubleTime(); }
        void End() override { Get<ChaosTimer>()->ToggleDoubleTime(); }

    public:
        explicit DoubleTime(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DoubleTime, {
    .effectName = "Double Time!",
    .description = "Well you've had first effect, but what about second effect?",
    .category = EffectType::Meta,
});