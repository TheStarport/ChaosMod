#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class Crt final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("CRT", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("CRT", false); }

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