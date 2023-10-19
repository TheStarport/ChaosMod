#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class RandomSystem final : public ActiveEffect
{
        void Begin() override { Teleporter::i()->WarpToRandomSystem(); }

    public:
        OneShotEffectInfo("Random System", EffectType::Teleport)
};
