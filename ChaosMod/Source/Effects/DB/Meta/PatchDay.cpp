#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ConfigManager.hpp"

#include "CoreComponents/PatchNotes.hpp"

class PatchDay final : public ActiveEffect
{
        void Begin() override { PatchNotes::GeneratePatch(PatchNotes::PatchVersion::Major); }
        bool CanSelect() override { return Get<ConfigManager>()->patchNotes.enable; }

    public:
        explicit PatchDay(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PatchDay, {
    .effectName = "Patch Day",
    .description = "The long awaited release has finally arrived. I wonder what changed this time?",
    .category = EffectType::Meta,
    .timingModifier = 0.0f,
    .isTimed = false
});