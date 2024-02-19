#pragma once

class OffsetHelper
{
    public:
        static bool IsInRandomMission()
        {
            static DWORD content = DWORD(GetModuleHandleA("content.dll"));
            return *reinterpret_cast<PDWORD>(content + 0x130C8C) != 0;
        }

        static bool IsInMission()
        {
            const auto fl = DWORD(GetModuleHandleA(nullptr));
            return !IsInRandomMission() && *PBOOL(fl + 0x27295C);
        }

        OffsetHelper() = delete;
};
