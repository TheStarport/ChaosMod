#pragma once

#include <Windows.h>

class OffsetHelper
{
    public:
        static bool IsInRandomMission()
        {
            static auto content = reinterpret_cast<unsigned long>(GetModuleHandleA("content.dll"));
            return *reinterpret_cast<unsigned long*>(content + 0x130C8C) != 0;
        }

        static bool IsInMission()
        {
            const auto fl = reinterpret_cast<unsigned long>(GetModuleHandleA(nullptr));
            return !IsInRandomMission() && *reinterpret_cast<bool*>(fl + 0x27295C);
        }

        static bool IsGamePaused() { return *reinterpret_cast<unsigned long*>(0x667D54) > 0; }

        static void ToggleGamePause(bool state) { *reinterpret_cast<unsigned long*>(0x667D54) = *reinterpret_cast<unsigned long*>(0x667D54) + (state ? 1 : -1); }

        OffsetHelper() = delete;
};
