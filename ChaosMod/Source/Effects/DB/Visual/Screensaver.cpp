#include "Components/ReshadeManager.hpp"
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Screensaver final : public ActiveEffect
{
    void Begin() override { Get<ReshadeManager>()->ToggleTechnique("Screensaver.fx", "Screensaver", true); }
    void End() override { Get<ReshadeManager>()->ToggleTechnique("Screensaver.fx", "Screensaver", false); }

    public:
        explicit Screensaver(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Screensaver, {
    .effectName = "Screensaver",
    .description = "You remember that DvD screensaver back in the day? This is that, but Freelancer.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});