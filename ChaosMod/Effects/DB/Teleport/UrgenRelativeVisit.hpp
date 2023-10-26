#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class UrgentRelativeVisit final : public ActiveEffect
{
        // Teleport the player to a random base
        void Begin() override
        {
            const auto random = Random::i()->Uniform(0u, 1u);
            Teleporter::i()->BeamToRandomBase(random);
        }

    public:
        const EffectInfo& GetEffectInfo() override
        {
            static const EffectInfo ef = EffectInfoBuilder("Urgent Relative Visit", EffectType::Teleport).WithOneShot().WithWeight(50).Create();
            return ef;
        }
};
