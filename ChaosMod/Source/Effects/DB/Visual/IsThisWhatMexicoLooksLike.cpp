#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class IsThisWhatMexicoLooksLike final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("Mexico.fx", "Mexico", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("Mexico.fx", "Mexico", false); }

    public:
        explicit IsThisWhatMexicoLooksLike(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(IsThisWhatMexicoLooksLike, {
    .effectName = "Is This What Mexico Looks Like?",
    .description = "'Mexico. Alls I'm sayin.'",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});