#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class Pong final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "Pong", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "Pong", false); }

    public:
        explicit Pong(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Pong, {
    .effectName = "The Hit Class: Pong",
    .description = "Why are you playing Freelancer when you could be playing the hit classic video game: Pong",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});