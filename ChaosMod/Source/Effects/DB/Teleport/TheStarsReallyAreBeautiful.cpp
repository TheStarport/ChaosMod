#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/Teleporter.hpp"

class TheStarsReallyAreBeautiful final : public ActiveEffect
{
        // Teleport the player to a star, with a 25% it being in a different system
        void Begin() override { Get<Teleporter>()->WarpToRandomStar(Get<Random>()->Uniform(0u, 3u) < 3); }

    public:
        explicit TheStarsReallyAreBeautiful(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(TheStarsReallyAreBeautiful, {
    .effectName = "The Stars Really Are Beautiful",
    .description = "Time to teleport you to your nearest star so you can admire it!",
    .category = EffectType::Teleport,
    .timingModifier = 0.0f,
    .isTimed = false
});
