#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class RandomSystem final : public ActiveEffect
{
        void Begin() override { Teleporter::i()->WarpToRandomSystem(); }

    public:
        const EffectInfo& GetEffectInfo() override
        {
            static const EffectInfo ef = EffectInfoBuilder("Random System", EffectType::Teleport).WithOneShot().WithWeight(50).Create();
            return ef;
        }
};
