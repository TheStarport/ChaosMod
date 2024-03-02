#include "PCH.hpp"

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
        explicit UrgentRelativeVisit(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(UrgentRelativeVisit, {
    .effectName = "Urgent Relative Visit",
    .description = "You got a text from Tobias to meet him on a random base. He said it was urgent...",
    .category = EffectType::Teleport,
    .timingModifier = 0.0f,
    .isTimed = false
});