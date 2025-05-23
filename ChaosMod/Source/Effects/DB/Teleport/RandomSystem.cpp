#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/Teleporter.hpp"

class RandomSystem final : public ActiveEffect
{
        void Begin() override { Get<Teleporter>()->WarpToRandomSystem(); }

    public:
        explicit RandomSystem(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(RandomSystem, {
    .effectName = "Random System",
    .description = "This place is boring, lets take you to a far more exotic land.",
    .category = EffectType::Teleport,
    .timingModifier = 0.0f,
    .isTimed = false
});