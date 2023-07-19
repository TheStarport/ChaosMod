#pragma once
#include "Effects/ActiveEffect.hpp"

class LonniganCameThrough final : public ActiveEffect
{
        void Begin() override { pub::Player::AdjustCash(1, 1'000'000); }

    public:
        DefEffectInfo("Lonnigan Came Through", 0.0f, EffectType::StatManipulation);
};
