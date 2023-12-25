#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class DrunkFlying final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("Drunk", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("Drunk", false); }

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