#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/ChaosTimer.hpp"

class DoubleEffectTime final : public ActiveEffect
{
        void Begin() override { ChaosTimer::i()->AdjustModifier(1.0f); }
        void End() override { ChaosTimer::i()->AdjustModifier(-1.0f); }

    public:
        DefEffectInfo("Double Effect Duration", 1.0f, EffectType::Meta);
};
