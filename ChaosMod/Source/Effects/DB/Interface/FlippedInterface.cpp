// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "Effects/ActiveEffect.hpp"

class FlippedUi final : public ActiveEffect
{
        void Begin() override
        {
            // TODO: Get<HudInterface>()->ToggleFlipping();
        }
        void End() override
        {
            // TODO: Get<HudInterface>()->ToggleFlipping();
        }

    public:
        explicit FlippedUi(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FlippedUi, {
    .effectName = "Flipped Interface",
    .description = "Causes most text on the screen to become inverted and display the text backwards.",
    .category = EffectType::Interface,
});