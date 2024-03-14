#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/CameraController.hpp"

class QuakeFov final : public ActiveEffect
{
        void Begin() override { Get<CameraController>()->OverrideFov(120.0f); }
        void End() override { Get<CameraController>()->OverrideFov(0.0f); }

    public:
        explicit QuakeFov(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(QuakeFov, {
    .effectName = "Quake FOV",
    .description = "We've adjusted your FOV to be more like another classic game. Generes don't matter do they?",
    .category = EffectType::Visual,
    .timingModifier = .75f,
    .exclusion = EffectExclusion::Visual
});