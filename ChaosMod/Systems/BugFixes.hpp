#pragma once

class BugFixes
{
        using sub62EE530DetourType = bool(__fastcall*)(uint** bm, void* edx, void* a2);
        inline static FunctionDetour<sub62EE530DetourType> sub62EE530Detour{ sub62EE530DetourType(DWORD(GetModuleHandleA("common.dll")) + 0x8E530) };

        static bool __fastcall Sub62EE530Detour(uint** bm, void* edx, void* a2)
        {
            uint* data = bm[1];
            if (!data || !data[1549] || !data[1552] || *PDWORD(DWORD(data) + 6196) > 13)
            {
                return false;
            }

            sub62EE530Detour.UnDetour();
            const auto res = sub62EE530Detour.GetOriginalFunc()(bm, edx, a2);
            sub62EE530Detour.Detour(Sub62EE530Detour);
            return res;
        }

    public:
        BugFixes() = delete;
        static void SetupDetours() { sub62EE530Detour.Detour(Sub62EE530Detour); };
};
