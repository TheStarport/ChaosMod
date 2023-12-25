#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class FilmGrain final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("FilmGrain", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("FilmGrain", false); }

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