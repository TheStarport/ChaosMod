#pragma once
#include "Systems/ShipManipulator.hpp"

class IllTrySpinning final : public ActiveEffect
{
        void Begin() override
        {
            ShipManipulator::i()->MakeShipsSpin(true);
            Utils::GetCShip()->get_behavior_interface()->update_current_behavior_auto_level(false);
        }

        void End() override
        {
            ShipManipulator::i()->MakeShipsSpin(false);
            auto ship = Utils::GetCShip();
            if (ship)
            {
                ship->get_behavior_interface()->update_current_behavior_auto_level(true);
            }
        }

    public:
        DefEffectInfo("I'll Try Spinning!", 1.0f, EffectType::Movement);
};
