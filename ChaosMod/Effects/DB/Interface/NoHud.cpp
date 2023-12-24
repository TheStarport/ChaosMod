// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class NoHud final : public ActiveEffect
{
        static void ToggleHud([[maybe_unused]] bool newState)
        {
            __asm {
                mov al, newState
	            mov	ds:[0x679c0c], al // the icons
	            mov	ds:[0x679c10], al // everything else
	            mov	ds:[0x679c40], al // scroll bars on weapon list
            }
        }

    public:
        void Begin() override { ToggleHud(false); }
        void End() override { ToggleHud(true); }
        explicit NoHud(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(NoHud, {
    .effectName = "No Hud",
    .description = "Disables your hud, but keeps your cursor (we didn't want to make it *too* hard).",
    .category = EffectType::Interface,
});