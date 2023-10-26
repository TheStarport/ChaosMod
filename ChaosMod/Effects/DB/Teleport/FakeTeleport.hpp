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
        const EffectInfo& GetEffectInfo() override
        {
            static const EffectInfo ef = EffectInfoBuilder("Fake Teleport", EffectType::Teleport).WithAbsoluteTime(5.0f).WithWeight(50).Create();
            return ef;
        }
};
