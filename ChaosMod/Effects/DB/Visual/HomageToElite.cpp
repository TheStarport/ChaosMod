#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/DirectX/Drawing.hpp"

class HomageToElite final : public ActiveEffect
{
        void FrameUpdate(float delta) override { Get<DrawingHelper>()->SetRenderState(D3DRS_FILLMODE, 2, true); }
        void End() override { Get<DrawingHelper>()->SetRenderState(D3DRS_FILLMODE, 3, true); }

    public:
        explicit HomageToElite(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(HomageToElite, {
    .effectName = "Homage To Elite",
    .description = "We've made the game look like Elite (the original one). It's a good look!",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});