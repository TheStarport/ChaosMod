#include "PCH.hpp"
#include "Systems/ChaosTimer.hpp"
#include "Systems/EventManager.hpp"
#include "Systems/UiManager.hpp"

static PBYTE thornLoadData;
typedef void*(__cdecl* ScriptLoadPtr)(const char*);
ScriptLoadPtr thornScriptLoad;

typedef void (*GlobalTimeFunc)(double delta);
unsigned char* data;
GlobalTimeFunc globalTimingFunction;

constexpr double SixtyFramesPerSecond = 1.0 / 60.0;
double timeCounter;

bool init = false;

void Init()
{
    init = true;
    ChaosTimer::RegisterAllEffects();
    EventManager::i()->SetupDetours();
}

void RequiredMemEdits()
{
    const auto fl = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));
    const auto common = reinterpret_cast<DWORD>(GetModuleHandleA("common.dll"));

    // Patch out vanilla cursor
    BYTE NopPatch[] = { 0x90, 0x90, 0x90, 0x90 };
    Utils::Memory::WriteProcMem(0x05B1750, NopPatch, 4);

    BYTE NopPatch2[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    Utils::Memory::WriteProcMem(0x5B32F2, NopPatch2, 7);
    Utils::Memory::WriteProcMem(0x5B1750, NopPatch2, 7);

    BYTE NopPatch3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    Utils::Memory::WriteProcMem(0x42025A, NopPatch3, 13);

    BYTE NopPatch4[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    Utils::Memory::WriteProcMem(0x41ECC7, NopPatch4, 16);

    BYTE NopPatch5[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    Utils::Memory::WriteProcMem(0x41EAA3, NopPatch5, 20);

    // BYTE NopPatch6[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    //  Utils::Memory::WriteProcMem(0x41F7D5, NopPatch6, 17);

    // Patch out checks for if the cursor is visible
    BYTE NopPatch7[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    Utils::Memory::WriteProcMem(0x004202F6, NopPatch7, 7);
    Utils::Memory::WriteProcMem(0x0041EEBD, NopPatch7, 7);
    Utils::Memory::WriteProcMem(0x00420474, NopPatch7, 7);

    // Patch out SetCursor
    Utils::Memory::NopAddress(0x598989, 7);
    Utils::Memory::NopAddress(0x5989B6, 7);
    Utils::Memory::NopAddress(0x598AF2, 7);
    Utils::Memory::NopAddress(0x598BEC, 7);
    Utils::Memory::NopAddress(0x598C0B, 7);

    // Set borderless window mode
    BYTE patch[] = { 0x00, 0x00 };
    constexpr DWORD borderlessWindowPatch1 = 0x02477A;
    constexpr DWORD borderlessWindowPatch2 = 0x02490D;
    Utils::Memory::WriteProcMem(fl + borderlessWindowPatch1, &patch, 2);
    Utils::Memory::WriteProcMem(fl + borderlessWindowPatch2, &patch, 2);

    // Ensure the game remains running when out of focus
    byte patch3[] = { 0xEB };
    Utils::Memory::WriteProcMem(fl + 0x1B2665, patch3, sizeof patch3);

    // Disable Multi-player
    std::array<byte, 20> disableMp = { 0xE8, 0x47, 0xE5, 0xEA, 0xFF, 0x83, 0x7C, 0x24, 0x50, 0x02, 0x75, 0x11, 0xDB, 0x05, 0x4C, 0x46, 0x57, 0x00, 0xEB, 0x0D };
    Utils::Memory::WriteProcMem(fl + 0x174634, disableMp.data(), disableMp.size());

    std::array<byte, 2> disableMp2 = { 0x00, 0x00 };
    Utils::Memory::WriteProcMem(fl + 0x174744, disableMp2.data(), disableMp2.size());

    // Cloaking devices can be internal equipment
    std::array<byte, 3> cloakingDevicesAsInternalEquipment = { 0x40, 0x17, 0x2D };
    Utils::Memory::WriteProcMem(common + 0x139B74, cloakingDevicesAsInternalEquipment.data(), cloakingDevicesAsInternalEquipment.size());
}

void* ScriptLoadHook(const char* script)
{
    if (!init)
    {
        Init();
    }

    UiManager::i()->SetCursor("arrow");

    Utils::Memory::UnDetour(reinterpret_cast<PBYTE>(thornScriptLoad), thornLoadData);
    void* res = thornScriptLoad(script);
    Utils::Memory::Detour(reinterpret_cast<PBYTE>(thornScriptLoad), ScriptLoadHook, thornLoadData);
    return res;
}

void __cdecl Update(const double delta)
{
    timeCounter += delta;
    while (timeCounter > SixtyFramesPerSecond)
    {
        ChaosTimer::i()->Update(static_cast<float>(SixtyFramesPerSecond));
        timeCounter -= SixtyFramesPerSecond;
    }

    Utils::Memory::UnDetour((PBYTE)globalTimingFunction, data);
    globalTimingFunction(delta);
    Utils::Memory::Detour((PBYTE)globalTimingFunction, Update, data);
}

void SetupHack()
{
    // make needed memory edits for chaos mod to work
    RequiredMemEdits();

    // Force our hook before the dx8 is created so we can inject DX9 like a boss
    UiManager::i();

    // Setup hooks
    HMODULE common = GetModuleHandleA("common");
    globalTimingFunction = reinterpret_cast<GlobalTimeFunc>(GetProcAddress(common, "?UpdateGlobalTime@Timing@@YAXN@Z"));
    data = static_cast<unsigned char*>(malloc(5));
    Utils::Memory::Detour(reinterpret_cast<PBYTE>(globalTimingFunction), Update, data);

    thornScriptLoad = reinterpret_cast<ScriptLoadPtr>(GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z"));
    thornLoadData = static_cast<PBYTE>(malloc(5));
    Utils::Memory::Detour(reinterpret_cast<PBYTE>(thornScriptLoad), ScriptLoadHook, thornLoadData);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    DisableThreadLibraryCalls(hModule);
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        SetupHack();
    }
    return TRUE;
}
