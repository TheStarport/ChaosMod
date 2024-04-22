#include "Components/ReshadeManager.hpp"
#include "PCH.hpp"

#include "DirectX/Drawing.hpp"
#include "Effects/ActiveEffect.hpp"

class PortraitMode final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("PortraitMode.fx", "PortraitMode", true); }
        void End() override { Get<ReshadeManager>()->ToggleTechnique("PortraitMode.fx", "PortraitMode", false); }

    public:
        explicit PortraitMode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PortraitMode, {
    .effectName = "Portrait Mode",
    .description = "You really shouldn't have let your mum record the gameplay...",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});