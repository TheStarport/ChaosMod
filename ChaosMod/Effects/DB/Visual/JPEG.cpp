#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class Jpeg final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("JPEG", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("JPEG", false); }

    public:
        explicit Jpeg(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Jpeg, {
    .effectName = "JPEG",
    .description = "Sorry we shared the game a few too many times, and now this is what the game looks like. This is the kind you'd get posted in a Facebook mum group that reshares Minion memes.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});