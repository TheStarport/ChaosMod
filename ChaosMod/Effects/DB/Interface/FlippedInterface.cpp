// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/HudInterface.hpp"

class FlippedUi final : public ActiveEffect
{
        void Begin() override { HudInterface::i()->ToggleFlipping(); }
        void End() override { HudInterface::i()->ToggleFlipping(); }

    public:
        explicit FlippedUi(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FlippedUi, {
    .effectName = "Flipped Interface",
    .description = "Causes most text on the screen to become inverted and display the text backwards.",
    .category = EffectType::Interface,
});