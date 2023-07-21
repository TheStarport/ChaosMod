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

    // Disable Multi-player
    const auto fl = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));
    std::array<byte, 20> disableMp = { 0xE8, 0x47, 0xE5, 0xEA, 0xFF, 0x83, 0x7C, 0x24, 0x50, 0x02, 0x75, 0x11, 0xDB, 0x05, 0x4C, 0x46, 0x57, 0x00, 0xEB, 0x0D };
    Utils::Memory::WriteProcMem(fl + 0x174634, disableMp.data(), disableMp.size());

    std::array<byte, 2> disableMp2 = { 0x00, 0x00 };
    Utils::Memory::WriteProcMem(fl + 0x174744, disableMp2.data(), disableMp2.size());
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
