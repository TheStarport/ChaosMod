#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class Nightvision final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "Nightvision", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "Nightvision", false); }

    public:
        explicit Nightvision(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Nightvision, {
    .effectName = "Nightvision",
    .description = "The game can be dark, these NV goggles should help.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});