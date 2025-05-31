// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "PCH.hpp"

#include "Components/HudInterface.hpp"
#include "Effects/ActiveEffect.hpp"

class CapNSlappy final : public ActiveEffect
{
        void Begin() override
        {
            ChaosMod::SetLanguage(static_cast<Language>(Get<Random>()->Uniform(static_cast<int>(Language::Chef), static_cast<int>(Language::Scottish))));
        }

    public:
        explicit CapNSlappy(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(CapNSlappy, {
    .effectName = "Cap'n Slappy",
    .description = "Arrrrrrrr, Avassst yee. With this effect who knows how unreadable the game will become.",
    .category = EffectType::Interface,
    .timingModifier = 0.0f,
    .isTimed = false
});