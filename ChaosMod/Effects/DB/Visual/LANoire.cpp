#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class LANoire final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("LANoire", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("LANoire", false); }

    public:
        explicit LANoire(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(LANoire, {
    .effectName = "L.A. Noire",
    .description = "When we make your game look like L.A. Noire, remember to do the voice. Talk like it's the 50s.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});