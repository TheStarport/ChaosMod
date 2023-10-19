#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/ChaosTimer.hpp"

class DoubleEffectTime final : public ActiveEffect
{
        void Begin() override { ChaosTimer::i()->AdjustModifier(1.0f); }
        void End() override { ChaosTimer::i()->AdjustModifier(-1.0f); }

    public:
        DefaultEffectInfo("Double Effect Duration", EffectType::Meta);
};
