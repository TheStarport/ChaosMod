#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Zoomies final : public ActiveEffect
{
        void Begin() override { EngineEquipConsts::CRUISING_SPEED *= 2.0f; }

        void End() override { EngineEquipConsts::CRUISING_SPEED *= 0.5f; }

    public:
        explicit Zoomies(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Zoomies, {
    .effectName = "Zoomies",
    .description = "We've overcharged your cruise engines. Make use of that high speed while you can!",
    .category = EffectType::Movement,
    .timingModifier = 1.5f
});