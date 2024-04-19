#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class Crt final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "CRT", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "CRT", false); }

    public:
        explicit Crt(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Crt, {
    .effectName = "Cathode-Ray Tube",
    .description = "Remember what it was like to play in your old CRT TV? The one you feel might have given you permenent eye damage?.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});