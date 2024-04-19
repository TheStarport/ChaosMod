#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class TurnTurtle final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "TurnTurtle", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "TurnTurtle", false); }

    public:
        explicit TurnTurtle(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(TurnTurtle, {
    .effectName = "Turn Turtle",
    .description = "Like a turtle stuck on its back, the whole world looks upside down.",
    .category = EffectType::Visual,
    .timingModifier = 0.5f
});