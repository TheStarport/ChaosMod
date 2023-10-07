// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "PCH.hpp"

#include "Systems/CameraController.hpp"
#include "Systems/ChaosTimer.hpp"
#include "Systems/EventManager.hpp"
#include "Systems/HudInterface.hpp"
#include "Systems/KeyManager.hpp"
#include "Systems/ShipManipulator.hpp"
#include "Systems/SystemComponents/TwitchVoting.hpp"
#include "Systems/UiManager.hpp"
#include "Utilities/AssetTracker.hpp"
#include <Systems/SystemComponents/SaveManager.hpp>

typedef void*(__cdecl* ScriptLoadPtr)(const char*);
typedef void (*GlobalTimeFunc)(double delta);

std::unique_ptr<FunctionDetour<ScriptLoadPtr>> thornLoadDetour;
std::unique_ptr<FunctionDetour<GlobalTimeFunc>> timingDetour;

constexpr float SixtyFramesPerSecond = 1.0f / 60.0f;
double timeCounter;

std::unordered_set<Archetype::Ship*> ships;

// template<typename T>
// struct Node
//{
//         Node* prev;
//         Node* childLeft;
//         Node* childRight;
//         uint key;
//         T value;
// };
//
// template<typename T>
// struct BinarySearchTree
//{
//         Node<T>* root;
//         Node<T>* unk;
//         Node<T>* unk2;
//
//     void TraverseTree(std::function<void(std::pair<uint, T> val)> func) { TraverseTree(root, nullptr, func); }
//
// private:
//     bool TraverseTree(Node<T>* node, const Node<T>* previousNode, std::function<void(std::pair<uint, T> val)> func)
//     {
//         if (node->arch == nullptr)
//         {
//             func(previousNode->key, previousNode->value);
//             return false;
//         }
//
//         Node<T>* nextNode = node->childLeft == previousNode ? node->prev : node->childLeft;
//         if (!TraverseTree(nextNode, node))
//         {
//             return TraverseTree(node->childRight, node);
//         }
//
//         return true;
//     }
// };
//
// bool TraverseTree(Node* node, const Node* previousNode)
//{
//     if (node->arch == nullptr)
//     {
//         ships.insert(previousNode->arch);
//         return false;
//     }
//
//     Node* nextNode = node->childLeft == previousNode ? node->prev : node->childLeft;
//     if (!TraverseTree(nextNode, node))
//     {
//         return TraverseTree(node->childRight, node);
//     }
//
//     return true;
// }
//
// DWORD* sub_62FCFB0(DWORD* DWORD_63FCAC0, DWORD hashPtr)
//{
//     LinkedListHeader* header = (LinkedListHeader*)DWORD_63FCAC0[1];
//
//     // result = DWORD_63FCAC0->h; // result = (*63FCAC0) + 4
//     // v3 = result->unk;               // v3 = (*(*63FCAC0) + 4) + 4
//     // v4 = DWORD_63FCAC0->end;
//
//     const size_t size = DWORD_63FCAC0[4];
//
//     Node<Archetype::Ship*>* root = header->root;
//     TraverseTree(root, nullptr);
//
//     printf("Hi");
//
//     // while (v3 != v4)
//     //{
//     //     struct P
//     //     {
//     //             uint i[4];
//     //     };
//     //     auto e0 = (P*)DWORD_63FCAC0;
//     //     auto e1 = (P*)v3;
//     //     auto e2 = (P*)v4;
//     //     auto e3 = (P*)result;
//
//     //    auto ship = (Archetype::Ship**)(DWORD(v3) + 0x10);
//
//     //    auto v6 = e1->i[0];
//     //    auto v7 = e1->i[2];
//
//     //    auto possibleNext = (Archetype::Ship**)(DWORD(v6) + 0x10);
//     //    auto possibleNext2 = (Archetype::Ship**)(DWORD(v7) + 0x10);
//
//     //    if (v3->hash >= hashPtr)
//     //    {
//     //        result = v3;
//     //        v3 = v3->prev;
//     //    }
//     //    else
//     //    {
//     //        v3 = v3->next; // v3 = (*v3) + 8)
//     //    }
//     //}
//     auto ship = (Archetype::Ship**)(DWORD(1) + 0x10);
//     return 0;
// }

bool init = false;
void Init()
{
    init = true;
    ChaosTimer::RegisterAllEffects();
    EventManager::i()->SetupDetours();
    CameraController::i();
    HudInterface::i();
    KeyManager::i();
    ShipManipulator::i();

    ChaosTimer::i()->InitEffects();

    DWORD* addr = PDWORD(0x063FCAC0);

    // DWORD* res = sub_62FCFB0(PDWORD(addr), 2151746432);
    // auto ss = (Archetype::Ship**)(DWORD(res) + 0x10);

    auto ships1 = (st6::map<uint, Archetype::Ship*>*)addr;
    auto ships2 = (st6::vector<Archetype::Ship*>*)addr;
    struct E
    {
            uint bytes[200];
    };

    const E* e = (E*)addr;

    auto test = (Archetype::Ship**)e->bytes[0];

    printf("hi");
}

const PDWORD screenWidth = ((PDWORD)0x679bc8);
const PDWORD screenHeight = ((PDWORD)0x679bcc);

uint width, height;

float CalcFov(float fDefaultFOV)
{
    constexpr float originalAspectRatio = 4.0f / 3.0f;

    const float newAspectRatio = static_cast<float>(*screenWidth) / static_cast<float>(*screenHeight);
    return static_cast<float>(atan(newAspectRatio / 2 / (originalAspectRatio / 2 / tan(fDefaultFOV * std::numbers::pi / 180))) * 180 / std::numbers::pi);
}

__declspec(naked) void LoadPerfIniNaked(void)
{
    __asm
    {
		mov ecx, [esp + 8]
		push ecx
		push [0x5D5088]

		mov ecx, 0x5B38D0
		call ecx

		mov ecx, [esp + 4]
		mov esi, width
		mov [ecx + 0x08], esi
		mov esi, height
		mov [ecx + 0x0C], esi

		add esp, 0x08
		ret
    }
}

void PatchResolution()
{
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    width = desktop.right - desktop.left;
    height = desktop.bottom - desktop.top;

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    if (aspectRatio > 2.5f || aspectRatio < 1.3333333f)
    {
        height = static_cast<long>(static_cast<float>(width) / 1.3333333f);
    }

    // Set the screen dimensions.
    MemUtils::WriteProcMem(screenWidth, &width, 4);
    MemUtils::WriteProcMem(screenHeight, &height, 4);

    // Set the FOV for the UI elements on the main screen.
    const float fov_win = CalcFov(27.216f);
    MemUtils::WriteProcMem(reinterpret_cast<char*>(0x59d5fd), &fov_win, 4);

    // Patch options menu to display extended resolutions.
    MemUtils::WriteProcMem(reinterpret_cast<char*>(0x4A9835), &width, 4);
    MemUtils::WriteProcMem(reinterpret_cast<char*>(0x4A9867), &height, 4);

    // single resolution.
    for (int i = 0, p = 0x4A9835; i < 5; i++, p += 10)
    {
        MemUtils::WriteProcMem(reinterpret_cast<char*>(p), &width, 4);
    }
    for (int i = 0, p = 0x4A9867; i < 5; i++, p += 10)
    {
        MemUtils::WriteProcMem(reinterpret_cast<char*>(p), &height, 4);
    }

    {
        // Remove aspect check when we add the widescreen resolution.
        constexpr std::array<byte, 1> patch = { 0xEB };
        MemUtils::WriteProcMem(reinterpret_cast<PCHAR>(GetModuleHandle(nullptr)) + 0xAF1E7, patch.data(), 1);

        // Patch energy gauge width
        const double patch4 = 0.166667f / aspectRatio;
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x5d7e58), &patch4, 9);

        // Patch news ticker width
        const float patch5 = 0.975f / aspectRatio;
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x5D3FB4), &patch5, 4);
    }

    // Set the screen dimensions.
    {
        const PBYTE fp = reinterpret_cast<PBYTE>(LoadPerfIniNaked) - 0x5B16CD - 5;
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x5B16CD) + 1, &fp, 4);
    }

    // Change the font sizes ~ Jason Hood. I gave up on my implementation
    // and used his.
    {
        static DWORD fontAdjustedWidth = *screenHeight * 4 / 3;
        if (aspectRatio >= 1.6)
        {
            fontAdjustedWidth = *screenHeight * 5 / 4;
        }

        const PDWORD adjustedWidth = &fontAdjustedWidth;
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x415cc7), &adjustedWidth, 4);
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x416663), &adjustedWidth, 4);
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x416913), &adjustedWidth, 4);
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x416be3), &adjustedWidth, 4);
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x412e8d), &adjustedWidth, 4);
    }

    // Change cinematic aspect ratio
    {
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x5C9020), &aspectRatio, 4);
    }

    // Adjust the nav map to use height not width
    {
        constexpr DWORD patch1 = 0x610854; // height not width
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x499A3B) + 1, &patch1, 4);

        // If greater than 1200 high then use NavMap1600
        constexpr DWORD patch2 = 1200; // 1600x1200
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x499A43) + 1, &patch2, 4);

        // If greater than/equal 864 high then use NavMap1280 (5D3EA0)
        const BYTE patch3[] = { 0x60, 0x03, 0x00, 0x00, 0x7C, 0x07, 0x68, 0xA0, 0x3E, 0x5D };
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x499A5C) + 1, &patch3, 10);

        // If greater/equal than 768 high then use NavMap1024 (5D3EB8)
        const BYTE patch4[] = { 0x00, 0x03, 0x00, 0x00, 0x7C, 0x0E, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x68, 0xB8, 0x3E, 0x5D };
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x499A6A) + 1, &patch4, 17);

        // Otherwise use 800x600 (5D3EAC)
        constexpr BYTE patch5[] = { 0xAC, 0x3E, 0x5D };
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x499A7F) + 1, &patch5, 3);
    }

    // Patch navmap to use 4/3 fov
    {
        const BYTE patch[] = { 0x68, 0x5d, 0xba, 0xd9, 0x41, 0x90, 0x90, 0x90, 0x90, 0x90 };
        MemUtils::WriteProcMem(reinterpret_cast<char*>(0x495B88), &patch, 10);
    }
}

void RequiredMemEdits()
{
    const auto fl = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));
    const auto common = reinterpret_cast<DWORD>(GetModuleHandleA("common.dll"));

    // Patch out vanilla cursor
    BYTE nopPatch[] = { 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x05B1750, nopPatch, 4);

    BYTE nopPatch2[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x5B32F2, nopPatch2, 7);
    MemUtils::WriteProcMem(0x5B1750, nopPatch2, 7);

    BYTE nopPatch3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x42025A, nopPatch3, 13);

    BYTE nopPatch4[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x41ECC7, nopPatch4, 16);

    BYTE nopPatch5[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x41EAA3, nopPatch5, 20);

    // BYTE NopPatch6[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    //  MemUtils::WriteProcMem(0x41F7D5, NopPatch6, 17);

    // Patch out checks for if the cursor is visible
    BYTE NopPatch7[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x004202F6, NopPatch7, 7);
    MemUtils::WriteProcMem(0x0041EEBD, NopPatch7, 7);
    MemUtils::WriteProcMem(0x00420474, NopPatch7, 7);

    // Patch out SetCursor
    MemUtils::NopAddress(0x598989, 7);
    MemUtils::NopAddress(0x5989B6, 7);
    MemUtils::NopAddress(0x598AF2, 7);
    MemUtils::NopAddress(0x598BEC, 7);
    MemUtils::NopAddress(0x598C0B, 7);

    // Set borderless window mode
    BYTE patch[] = { 0x00, 0x00 };
    constexpr DWORD borderlessWindowPatch1 = 0x02477A;
    constexpr DWORD borderlessWindowPatch2 = 0x02490D;
    MemUtils::WriteProcMem(fl + borderlessWindowPatch1, &patch, 2);
    MemUtils::WriteProcMem(fl + borderlessWindowPatch2, &patch, 2);

    // Ensure the game remains running when out of focus
    byte patch3[] = { 0xEB };
    MemUtils::WriteProcMem(fl + 0x1B2665, patch3, sizeof patch3);

    // Disable Multi-player
    std::array<byte, 20> disableMp = { 0xE8, 0x47, 0xE5, 0xEA, 0xFF, 0x83, 0x7C, 0x24, 0x50, 0x02, 0x75, 0x11, 0xDB, 0x05, 0x4C, 0x46, 0x57, 0x00, 0xEB, 0x0D };
    MemUtils::WriteProcMem(fl + 0x174634, disableMp.data(), disableMp.size());

    std::array<byte, 2> disableMp2 = { 0x00, 0x00 };
    MemUtils::WriteProcMem(fl + 0x174744, disableMp2.data(), disableMp2.size());

    // Cloaking devices can be internal equipment
    std::array<byte, 3> cloakingDevicesAsInternalEquipment = { 0x40, 0x17, 0x2D };
    MemUtils::WriteProcMem(common + 0x139B74, cloakingDevicesAsInternalEquipment.data(), cloakingDevicesAsInternalEquipment.size());

    std::array<byte, 1> holdDownMissiles = { 0x0 };
    MemUtils::WriteProcMem(fl + 0x11D281, holdDownMissiles.data(), 1);

    std::array<byte, 1> planetsKeepOnSpinning = { 0xEB };
    MemUtils::WriteProcMem(common + 0x0E698E, planetsKeepOnSpinning.data(), 1);

    float nearPlaneFrustum = 0.05f;
    MemUtils::WriteProcMem(fl + 0x210530, &nearPlaneFrustum, sizeof(float));

    ProtectExecuteReadWrite(reinterpret_cast<void*>(0x46b650), 5);
    *reinterpret_cast<PBYTE>(0x46b650) = 0xe9;
    *reinterpret_cast<PDWORD>(0x46b651) = static_cast<DWORD>(0x46b580) - static_cast<DWORD>(0x46b651) - 4;

    // bypass the single player tests preventing chat
    std::array<byte, 1> bypassSpChecks = { 0x0 };
    MemUtils::WriteProcMem(0x46a11f + 1, bypassSpChecks.data(), 1);
    MemUtils::WriteProcMem(0x437374 + 1, bypassSpChecks.data(), 1);
    MemUtils::WriteProcMem(0x54ae67 + 1, bypassSpChecks.data(), 1);
    MemUtils::WriteProcMem(0x574b91 + 1, bypassSpChecks.data(), 1);
    MemUtils::WriteProcMem(0x574c43 + 1, bypassSpChecks.data(), 1);
    MemUtils::WriteProcMem(0x574f74 + 1, bypassSpChecks.data(), 1);

    // Remove cruise speed display limit
    std::array<byte, 2> removeCruiseSpeedDisplayLimit = { 0x90, 0xE9 };
    MemUtils::WriteProcMem(0x0D5936, removeCruiseSpeedDisplayLimit.data(), 2);

    PatchResolution();
}

void* ScriptLoadHook(const char* script)
{
    if (!init)
    {
        Init();
    }

    UiManager::i()->SetCursor("arrow");

    thornLoadDetour->UnDetour();
    const auto res = thornLoadDetour->GetOriginalFunc()(script);
    thornLoadDetour->Detour(ScriptLoadHook);
    return res;
}

void __cdecl Update(const double delta)
{
    timeCounter += delta;
    while (timeCounter > SixtyFramesPerSecond)
    {
        SaveManager::SaveTimer(SixtyFramesPerSecond);
        ChaosTimer::i()->Update(SixtyFramesPerSecond);
        timeCounter -= SixtyFramesPerSecond;
    }

    TwitchVoting::i()->Poll();
    ChaosTimer::i()->FrameUpdate(static_cast<float>(delta));
    timingDetour->UnDetour();
    timingDetour->GetOriginalFunc()(delta);
    timingDetour->Detour(Update);
}

void SetupHack()
{
    AssetTracker::StartDetours();

    // make needed memory edits for chaos mod to work
    RequiredMemEdits();

    // Force our hook before the dx8 is created so we can inject DX9 like a boss
    UiManager::i();

    // Setup hooks
    const HMODULE common = GetModuleHandleA("common");
    timingDetour =
        std::make_unique<FunctionDetour<GlobalTimeFunc>>(reinterpret_cast<GlobalTimeFunc>(GetProcAddress(common, "?UpdateGlobalTime@Timing@@YAXN@Z")));
    thornLoadDetour =
        std::make_unique<FunctionDetour<ScriptLoadPtr>>(reinterpret_cast<ScriptLoadPtr>(GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z")));

    timingDetour->Detour(Update);
    thornLoadDetour->Detour(ScriptLoadHook);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    DisableThreadLibraryCalls(hModule);
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        float newX = 0.5f;
        float newY = 0.5f;

        MemUtils::WriteProcMem(0x4dd493, &newX, sizeof(float));
        MemUtils::WriteProcMem(0x4dd49b, &newY, sizeof(float));
        SetupHack();
    }
    return TRUE;
}
