#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class Pong final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("Pong", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("Pong", false); }

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