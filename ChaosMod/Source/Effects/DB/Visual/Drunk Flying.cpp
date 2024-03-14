#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class DrunkFlying final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("Drunk", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("Drunk", false); }

    public:
        explicit DrunkFlying(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DrunkFlying, {
    .effectName = "Drunk Flying",
    .description = "You really shouldn't have had that last quadruple vodka and lime. The first 7 were probably okay though, right?",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});