#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class PictureInPicture final : public ActiveEffect
{
        void Begin() override { Get<ReshadeManager>()->ToggleTechnique("PictureInPicture.fx", "PictureInPicture", true); }

        void End() override { Get<ReshadeManager>()->ToggleTechnique("PictureInPicture.fx", "PictureInPicture", false); }

    public:
        explicit PictureInPicture(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PictureInPicture, {
    .effectName = "Picture In Picture",
    .description = "Your game is so good, you get to see it twice.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});