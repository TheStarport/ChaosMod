#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class TakeABreak final : public ActiveEffect
{
        void Begin() override { Teleporter::i()->SaveCurrentPosition(); }

        void End() override { Teleporter::i()->RestorePreviousPosition(); }

    public:
        DefEffectInfo("Take A Break", 2.0f, EffectType::Teleport)
};
