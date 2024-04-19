#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class FilmGrain final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "FilmGrain", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "FilmGrain", false); }

    public:
        explicit FilmGrain(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FilmGrain, {
    .effectName = "Film Grain",
    .description = "Like your in an old western. A very old western. Where the original tape was left outside to soak. For a year.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});