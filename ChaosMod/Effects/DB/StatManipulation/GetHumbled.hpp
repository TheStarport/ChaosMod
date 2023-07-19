#pragma once
#include "Effects/ActiveEffect.hpp"

class GetHumbled final : public ActiveEffect
{
        void Begin() override
        {
            int cash;
            pub::Player::InspectCash(1, cash);

            const int newCash = cash * 10 / 100;
            pub::Player::AdjustCash(1, -cash + newCash);
        }

    public:
        DefEffectInfo("Get Humbled", 0.0f, EffectType::StatManipulation);
};
