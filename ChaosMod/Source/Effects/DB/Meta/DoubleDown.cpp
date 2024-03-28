#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ConfigManager.hpp"

#include "CoreComponents/PatchNotes.hpp"

class PatchDay final : public ActiveEffect
{
        void Begin() override { PatchNotes::DoubleDown(); }
        bool CanSelect() override { return Get<ConfigManager>()->patchNotes.enable && !PatchNotes::GetPatchNotes().empty(); }

    public:
        explicit PatchDay(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PatchDay, {
    .effectName = "Double Down",
    .description = "That last patch was epic! Lets double its effects!",
    .category = EffectType::Meta,
    .timingModifier = 0.0f,
    .isTimed = false
});