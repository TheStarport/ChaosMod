#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ShipManipulator.hpp"

class CocaineMode final : public ActiveEffect
{
        void Begin() override { Get<ShipManipulator>()->SetCocaineMode(true); }
        void End() override { Get<ShipManipulator>()->SetCocaineMode(false); }

    public:
        explicit CocaineMode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(CocaineMode, {
    .effectName = "Cocaine Mode",
    .description = "We gave you and every other pilot copius amounts of powered joy. The game may be harder to play like this...",
    .category = EffectType::Movement
});