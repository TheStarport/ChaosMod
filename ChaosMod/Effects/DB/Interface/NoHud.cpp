// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/UiManager.hpp"
#include "Utilities/OffsetHelper.hpp"

class NoHud final : public ActiveEffect
{

        HCURSOR cursor = nullptr;
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
        void Begin() override
        {
            ToggleHud(false);
            UiManager::i()->OverrideCursor(cursor);
        }

        void End() override
        {
            ToggleHud(true);
            UiManager::i()->OverrideCursor(std::nullopt);
        };

        explicit NoHud(const EffectInfo& effectInfo) : ActiveEffect(effectInfo)
        {
            // Ensure guns still track while hud is disabled
            std::array<byte, 2> patch = { 0x66, 0x40 }; // INC AX
            const auto fl = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));
            MemUtils::WriteProcMem(fl + 0xEC48D, patch.data(), patch.size());
            MemUtils::WriteProcMem(fl + 0x11DACD, patch.data(), patch.size());

            cursor = LoadCursorFromFileA("../DATA/CHAOS/null.cur");
        }
};

// clang-format off
SetupEffect(NoHud, {
    .effectName = "No Hud",
    .description = "Disables your hud, but keeps your cursor (we didn't want to make it *too* hard).",
    .category = EffectType::Interface,
});