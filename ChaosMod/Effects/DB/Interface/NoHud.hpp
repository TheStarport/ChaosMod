// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

class NoHud final : public ActiveEffect
{
        static void ToggleHud([[maybe_unused]] bool newState)
        {
            __asm {
                mov al, newState
                cmp	ds:[0x679c0c], al
	            jnz	HUD
	            cmp	ds:[0x679c20], al
	            sete	al
              HUD:
	            mov	ds:[0x679c0c], al // the icons
	            mov	ds:[0x679c10], al // everything else
	            mov	ds:[0x679c40], al // scroll bars on weapon list
            }
        }

    public:
        void Begin() override { ToggleHud(false); }
        void End() override { ToggleHud(true); }

        DefEffectInfo("No Hud", 1.0f, EffectType::Interface);
};
