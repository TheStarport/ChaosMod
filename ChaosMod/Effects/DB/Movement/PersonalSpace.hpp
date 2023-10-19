#pragma once
#include "Systems/ShipManipulator.hpp"

class PersonalSpace final : public ActiveEffect
{
        void Begin() override { ShipManipulator::i()->SetPersonalSpace(true); }
        void End() override { ShipManipulator::i()->SetPersonalSpace(false); }

    public:
        DefaultEffectInfo("Personal Space", EffectType::Movement);
};
