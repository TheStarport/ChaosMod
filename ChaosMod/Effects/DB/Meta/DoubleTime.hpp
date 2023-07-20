#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/ChaosTimer.hpp"

class DoubleTime final : public ActiveEffect
{
        void Begin() override { ChaosTimer::i()->ToggleDoubleTime(); }
        void End() override { ChaosTimer::i()->ToggleDoubleTime(); }

    public:
        DefEffectInfo("Double Time", 1.0f, EffectType::Meta);
};
