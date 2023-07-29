#pragma once
#include "Effects/ActiveEffect.hpp"

class CoolantLeak final : public ActiveEffect
{
        void Update(float delta) override
        {
            CShip* ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            ship->set_power(0.0f);
        }

    public:
        DefEffectInfo("Coolant Leak", 1.0f, EffectType::StatManipulation);
};
