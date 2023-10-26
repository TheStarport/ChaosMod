#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class TakeABreak final : public ActiveEffect
{
        void Begin() override { Teleporter::i()->SaveCurrentPosition(); }

        void End() override { Teleporter::i()->RestorePreviousPosition(); }

    public:
        const EffectInfo& GetEffectInfo() override
        {
            static const EffectInfo ef = EffectInfoBuilder("Take A Break", EffectType::Teleport).WithRelativeTime(2.0f).WithWeight(50).Create();
            return ef;
        }
};
