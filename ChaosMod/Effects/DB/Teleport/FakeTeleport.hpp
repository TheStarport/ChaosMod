#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class FakeTeleport final : public ActiveEffect
{
        void Begin() override
        {
            Teleporter::i()->SaveCurrentPosition();
            Teleporter::i()->WarpToRandomSolar(false);
        }

        void End() override { Teleporter::i()->RestorePreviousPosition(); }

    public:
        AbsoluteEffectInfo("Fake Teleport", EffectType::Teleport, 5.0f)
};
