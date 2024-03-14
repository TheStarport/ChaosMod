#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ChaosTimer.hpp"

class DoubleEffectTime final : public ActiveEffect
{
        void Begin() override { Get<ChaosTimer>()->AdjustModifier(1.0f); }
        void End() override { Get<ChaosTimer>()->AdjustModifier(-1.0f); }

    public:
        explicit DoubleEffectTime(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DoubleEffectTime, {
    .effectName = "Double Effect Duration",
    .description = "Exactly what is says on the tin dumb dumb",
    .category = EffectType::Meta,
});