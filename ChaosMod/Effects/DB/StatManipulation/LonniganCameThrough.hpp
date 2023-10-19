#pragma once
#include "Effects/ActiveEffect.hpp"

class LonniganCameThrough final : public ActiveEffect
{
        void Begin() override { pub::Player::AdjustCash(1, 1'000'000); }

    public:
        OneShotEffectInfo("Lonnigan Came Through", EffectType::StatManipulation);
};
