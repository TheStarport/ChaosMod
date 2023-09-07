#include "PCH.hpp"

#include "Systems/CameraController.hpp"
#include "Systems/ChaosTimer.hpp"
#include "Systems/EventManager.hpp"
#include "Systems/HudInterface.hpp"
#include "Systems/KeyManager.hpp"
#include "Systems/ShipManipulator.hpp"
#include "Systems/UiManager.hpp"

typedef void*(__cdecl* ScriptLoadPtr)(const char*);
typedef void (*GlobalTimeFunc)(double delta);

std::unique_ptr<FunctionDetour<ScriptLoadPtr>> thornLoadDetour;
std::unique_ptr<FunctionDetour<GlobalTimeFunc>> timingDetour;

constexpr double SixtyFramesPerSecond = 1.0 / 60.0;
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

    E* e = (E*)addr;

    auto test = (Archetype::Ship**)e->bytes[0];

    printf("hi");
}

void RequiredMemEdits()
{
    const auto fl = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));
    const auto common = reinterpret_cast<DWORD>(GetModuleHandleA("common.dll"));

    // Patch out vanilla cursor
    BYTE NopPatch[] = { 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x05B1750, NopPatch, 4);

    BYTE NopPatch2[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x5B32F2, NopPatch2, 7);
    MemUtils::WriteProcMem(0x5B1750, NopPatch2, 7);

    BYTE NopPatch3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x42025A, NopPatch3, 13);

    BYTE NopPatch4[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x41ECC7, NopPatch4, 16);

    BYTE NopPatch5[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    MemUtils::WriteProcMem(0x41EAA3, NopPatch5, 20);

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

    ProtectExecuteReadWrite((void*)0x46b650, 5);
    *(PBYTE)(0x46b650) = 0xe9;
    *(PDWORD)(0x46b651) = (DWORD)(0x46b580) - (DWORD)(0x46b651) - 4;

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
        ChaosTimer::i()->Update(static_cast<float>(SixtyFramesPerSecond));
        timeCounter -= SixtyFramesPerSecond;
    }

    ChaosTimer::i()->FrameUpdate(static_cast<float>(SixtyFramesPerSecond));
    timingDetour->UnDetour();
    timingDetour->GetOriginalFunc()(delta);
    timingDetour->Detour(Update);
}

void SetupHack()
{
    // make needed memory edits for chaos mod to work
    RequiredMemEdits();

    // Force our hook before the dx8 is created so we can inject DX9 like a boss
    UiManager::i();

    // Setup hooks
    HMODULE common = GetModuleHandleA("common");
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
