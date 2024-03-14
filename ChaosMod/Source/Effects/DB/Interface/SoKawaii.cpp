// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/HudInterface.hpp"

class SoKawaii final : public ActiveEffect
{
        void Begin() override { ChaosMod::SetLanguage(Language::UwU); }

    public:
        explicit SoKawaii(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SoKawaii, {
    .effectName = "So Kawaiiiiiiii",
    .description = "It's like we're in an anime, and reading in that voice should hurt...",
    .category = EffectType::Interface,
    .timingModifier = 0.0f,
    .isTimed = false
});