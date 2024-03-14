// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "ImGui/ImGuiManager.hpp"

class RollCredits final : public ActiveEffect
{
        void Begin() override { ImGuiManager::StartCredits(); }

    public:
        explicit RollCredits(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(RollCredits, {
    .effectName = "Roll Credits",
    .description = "Causes the credits of the game to scroll down the screen while you play. Reminder, Chris Roberts deserves almost no credit for this game.",
    .category = EffectType::Interface,
    .timingModifier = 0.0f,
    .isTimed = false,
});
