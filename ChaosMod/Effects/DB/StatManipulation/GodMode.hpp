#pragma once
#include "Effects/ActiveEffect.hpp"

class GodMode final : public ActiveEffect
{
        void Begin() override
        {
            const auto ship = Utils::GetCShip();
            pub::SpaceObj::SetInvincible(ship->iID, true, true, 0);
        }

        void End() override
        {
            if (const auto ship = Utils::GetCShip())
            {
                pub::SpaceObj::SetInvincible(ship->iID, false, false, 0);
            }
        }

    public:
        DefEffectInfo("God Mode", 0.5f, EffectType::StatManipulation);
};
