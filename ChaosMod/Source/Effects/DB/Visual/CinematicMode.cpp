#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class CinematicMode final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("Vignette", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("Vignette", false); }

    public:
        explicit CinematicMode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(CinematicMode, {
    .effectName = "Cinematic Mode",
    .description = "We'll dim the lights all ready for your monologue.",
    .category = EffectType::Visual
});