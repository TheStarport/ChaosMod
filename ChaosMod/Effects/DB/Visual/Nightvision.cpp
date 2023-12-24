#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class Nightvision final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("Nightvision", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("Nightvision", false); }

    public:
        explicit Nightvision(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Nightvision, {
    .effectName = "Nightvision",
    .description = "The game can be dark, these NV goggles should help.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});