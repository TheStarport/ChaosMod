#pragma once
#include "Systems/ShipManipulator.hpp"

class PersonalSpace final : public ActiveEffect
{
        void Begin() override { ShipManipulator::i()->SetPersonalSpace(true); }
        void End() override { ShipManipulator::i()->SetPersonalSpace(false); }

    public:
        DefEffectInfo("Personal Space", 1.0f, EffectType::Movement);
};
