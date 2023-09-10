#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class TheStarsReallyAreBeautiful final : public ActiveEffect
{
        // Teleport the player to a star, with a 25% it being in a different system
        void Begin() override { Teleporter::i()->WarpToRandomStar(Random::i()->Uniform(0u, 3u) < 3); }

    public:
        DefEffectInfo("The Stars Really Are Beautiful", 0.0f, EffectType::Teleport)
};
