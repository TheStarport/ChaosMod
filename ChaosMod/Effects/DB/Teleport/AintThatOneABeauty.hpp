#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class AintThatOneABeauty final : public ActiveEffect
{
        // Teleport the player to a solar, with a 25% it being in a different system
        void Begin() override { Teleporter::i()->WarpToRandomSolar(Random::i()->Uniform(0u, 3u) < 3); }

    public:
        OneShotEffectInfo("Ain't That One A Beauty", EffectType::Teleport)
};
