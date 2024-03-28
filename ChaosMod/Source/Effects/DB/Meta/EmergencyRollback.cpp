#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ConfigManager.hpp"

#include "CoreComponents/PatchNotes.hpp"

class EmergencyRollback final : public ActiveEffect
{
        void Begin() override { PatchNotes::RevertLastPatch(); }
        bool CanSelect() override { return Get<ConfigManager>()->patchNotes.enable; }

    public:
        explicit EmergencyRollback(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(EmergencyRollback, {
    .effectName = "Emergancy Rollback",
    .description = "The game is in a horrible state right now. The last patch was a mistake.",
    .category = EffectType::Meta,
    .timingModifier = 0.0f,
    .isTimed = false
});