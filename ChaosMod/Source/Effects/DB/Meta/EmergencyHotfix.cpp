#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ConfigManager.hpp"

#include "CoreComponents/PatchNotes.hpp"

class EmergencyHotfix final : public ActiveEffect
{
        void Begin() override { PatchNotes::GeneratePatch(); }
        bool CanSelect() override { return Get<ConfigManager>()->patchNotes.enable; }

    public:
        explicit EmergencyHotfix(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(EmergencyHotfix, {
    .effectName = "Emergancy Hotfix",
    .description = "The game is in a horrible state right. This patch should hopefully fix all problems.",
    .category = EffectType::Meta,
    .timingModifier = 0.0f,
    .isTimed = false
});