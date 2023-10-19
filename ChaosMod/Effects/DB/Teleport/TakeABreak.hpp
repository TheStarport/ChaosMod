#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class TakeABreak final : public ActiveEffect
{
        void Begin() override { Teleporter::i()->SaveCurrentPosition(); }

        void End() override { Teleporter::i()->RestorePreviousPosition(); }

    public:
        RelativeEffectInfo("Take A Break", EffectType::Teleport, 2.0f)
};
