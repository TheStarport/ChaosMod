#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class UrgentRelativeVisit final : public ActiveEffect
{
        // Teleport the player to a random base
        void Begin() override { Teleporter::i()->BeamToRandomBase(false); }

    public:
        OneShotEffectInfo("Urgent Relative Visit", EffectType::Teleport)
};
