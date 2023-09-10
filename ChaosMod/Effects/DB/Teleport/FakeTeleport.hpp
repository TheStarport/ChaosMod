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
        DefEffectInfoFixed("Fake Teleport", 7.0f, EffectType::Teleport)
};
