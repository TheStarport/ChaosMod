#pragma once
#include "Effects/ActiveEffect.hpp"

class NeverSayNoToBacta final : public ActiveEffect
{
        float timer = 1.0f;
        void Update(float delta) override
        {
            const auto ship = Utils::GetCShip();
            timer -= delta;
            if (!ship || timer > 0.0f)
            {
                return;
            }

            timer = 1.0f;
            float health;
            pub::SpaceObj::GetRelativeHealth(ship->get_id(), health);
            if (health < 1.0f)
            {
                pub::SpaceObj::SetRelativeHealth(ship->get_id(), health + 0.01f);
            }
        }

    public:
        DefaultEffectInfo("Never Say No To Bacta", EffectType::StatManipulation);
};
