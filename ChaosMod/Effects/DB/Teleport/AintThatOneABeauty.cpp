#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class AintThatOneABeauty final : public ActiveEffect
{
        // Teleport the player to a solar, with a 25% it being in a different system
        void Begin() override { Get<Teleporter>()->WarpToRandomSolar(Get<Random>()->Uniform(0u, 3u) < 3); }

    public:
        explicit AintThatOneABeauty(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(AintThatOneABeauty, {
    .effectName = "Ain't That One A Beauty?",
    .description = "We've teleported you to a random solar so you can inspect and admire it. Aren't we nice?",
    .category = EffectType::Teleport,
    .timingModifier = 0.0f,
    .isTimed = false
});