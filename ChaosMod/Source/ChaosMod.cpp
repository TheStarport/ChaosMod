// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "PCH.hpp"

#include "../Include/CoreComponents/TwitchVoting.hpp"
#include "Components/CameraController.hpp"
#include "Components/DiscordManager.hpp"
#include "Components/EventManager.hpp"
#include "Components/GlobalTimers.hpp"
#include "Components/HudInterface.hpp"
#include "Components/KeyManager.hpp"
#include "Components/MoviePlayer.hpp"
#include "Components/PersonalityHelper.hpp"
#include "Components/ReshadeManager.hpp"
#include "Components/SaveManager.hpp"
#include "Components/ShipManipulator.hpp"
#include "Components/SpaceObjectSpawner.hpp"
#include "Components/SplashScreen.hpp"
#include "Components/Teleporter.hpp"
#include "Components/UiManager.hpp"

#include "CoreComponents/PatchNotes.hpp"
#include "CoreComponents/ChaosTimer.hpp"

#include "Memory/BugFixes.hpp"
#include "Memory/OnHit.hpp"
#include "Memory/OnSound.hpp"
#include "Memory/ShipInfocardOverride.hpp"

#include "Memory/AssetTracker.hpp"

#include <reshade.hpp>

const st6_malloc_t st6_malloc = reinterpret_cast<st6_malloc_t>(GetProcAddress(GetModuleHandleA("msvcrt.dll"), "malloc")); // NOLINT
const st6_free_t st6_free = reinterpret_cast<st6_free_t>(GetProcAddress(GetModuleHandleA("msvcrt.dll"), "free"));         // NOLINT

using ScriptLoadPtr = void*(* )(const char*);
using GlobalTimeFunc = void (*)(double delta);

std::unique_ptr<FunctionDetour<ScriptLoadPtr>> thornLoadDetour;
std::unique_ptr<FunctionDetour<GlobalTimeFunc>> timingDetour;

using CreateIdFunc = uint (*)(const char* str);
std::unique_ptr<FunctionDetour<CreateIdFunc>> createIdDetour;
FILE* hashFile = nullptr;
std::map<std::string, uint> hashMap;

std::optional<std::string> ChaosMod::HashLookup(const uint hash)
{
    for (const auto& [nickname, existingHash] : hashMap)
    {
        if (existingHash == hash)
        {
            return nickname;
        }
    }

    return std::nullopt;
}

uint CreateIdDetour(const char* string)
{
    if (!string)
    {
        return 0;
    }

    if (const auto found = hashMap.find(string); found != hashMap.end())
    {
        return found->second;
    }

    createIdDetour->UnDetour();
    const uint hash = CreateID(string);

    if (const std::string str = string; !hashMap.contains(str))
    {
        hashMap[str] = hash;
#ifdef _DEBUG
        if (!hashFile)
        {
            (void)fopen_s(&hashFile, "hashmap.csv", "wb");
        }

        (void)fprintf_s(hashFile, "%s,%u,0x%X\n", string, hash, hash);
        (void)fflush(hashFile);
#endif
    }

    createIdDetour->Detour(CreateIdDetour);
    return hash;
}

constexpr float SixtyFramesPerSecond = 1.0f / 60.0f;
double timeCounter;

void ChaosMod::DelayedInit()
{
    // We no longer need the splash screen
    ResetComponent<SplashScreen>();

    // Register all the components we can!
    SetComponent<EventManager>();
    SetComponent<CameraController>();
    SetComponent<MoviePlayer>();
    SetComponent<HudInterface>();
    SetComponent<ShipManipulator>();
    SetComponent<Teleporter>();
    SetComponent<TwitchVoting>();
    SetComponent<ChatConsole>();
    SetComponent<PersonalityHelper>();
    SetComponent<SpaceObjectSpawner>();

    Get<ChaosTimer>()->InitEffects();

    PatchNotes::LoadPatches();
    // Apply the patches for the first time
    PatchNotes::ResetPatches(true, false);
    OnSound::Init();
    ShipInfocardOverride::Init();
    Get<EventManager>()->SetupDetours();
}

auto* screenWidth = reinterpret_cast<const DWORD*>(0x679bc8);
auto* screenHeight = reinterpret_cast<const DWORD*>(0x679bcc);

uint width, height;

float CalcFov(float defaultFov)
{
    constexpr float originalAspectRatio = 4.0f / 3.0f;

    const float newAspectRatio = static_cast<float>(*screenWidth) / static_cast<float>(*screenHeight);
    return static_cast<float>(atan(newAspectRatio / 2 / (originalAspectRatio / 2 / tan(defaultFov * std::numbers::pi / 180))) * 180 / std::numbers::pi);
}

float __stdcall Fovx(PBYTE camera) { return CalcFov((camera == (PBYTE)0x67dbf8) ? 27.216f : 35.0f); }

__declspec(naked) void HkCb_Fovx_Naked(void)
{
    __asm
    {
		push esi
		call Fovx
		push 0x40F623
		ret
    }
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
    MemUtils::WriteProcMem(reinterpret_cast<DWORD>(screenWidth), &width, 4);
    MemUtils::WriteProcMem(reinterpret_cast<DWORD>(screenHeight), &height, 4);

    // Disable the reading of the znear/zfar/fovy parameters.
    {
        std::array<byte, 1> patch = { 0x00 };
        MemUtils::WriteProcMem(0x5C8994, patch.data(), 1);
        MemUtils::WriteProcMem(0x5C899C, patch.data(), 1);
        MemUtils::WriteProcMem(0x5C89A4, patch.data(), 1);
    }

    // Force the fovx to predefined values.
    {
        std::array<byte, 2> patch= { 0x90, 0xE9 };
        MemUtils::WriteProcMem(0x40f617, patch.data(), 2);
        PBYTE fovX = reinterpret_cast<PBYTE>(HkCb_Fovx_Naked) - 0x40f618 - 5;
        MemUtils::WriteProcMem(0x40f618 + 1, &fovX, 4);
    }

    // Set the FOV for the UI elements on the main screen.
    const float fov_win = CalcFov(27.216f);
    MemUtils::WriteProcMem(0x59d5fd, &fov_win, 4);

    // Patch options menu to display extended resolutions.
    MemUtils::WriteProcMem(0x4A9835, &width, 4);
    MemUtils::WriteProcMem(0x4A9867, &height, 4);

    // single resolution.
    for (int i = 0, p = 0x4A9835; i < 5; i++, p += 10)
    {
        MemUtils::WriteProcMem(p, &width, 4);
    }
    for (int i = 0, p = 0x4A9867; i < 5; i++, p += 10)
    {
        MemUtils::WriteProcMem(p, &height, 4);
    }

    {
        // Remove aspect check when we add the widescreen resolution.
        constexpr std::array<byte, 1> patch = { 0xEB };
        MemUtils::WriteProcMem(reinterpret_cast<DWORD>(GetModuleHandle(nullptr)) + 0xAF1E7, patch.data(), 1);

        // Patch energy gauge width
        const double patch4 = 0.166667f / aspectRatio;
        MemUtils::WriteProcMem(0x5d7e58, &patch4, 9);

        // Patch news ticker width
        const float patch5 = 0.975f / aspectRatio;
        MemUtils::WriteProcMem(0x5D3FB4, &patch5, 4);
    }

    // Set the screen dimensions.
    {
        const PBYTE fp = reinterpret_cast<PBYTE>(LoadPerfIniNaked) - 0x5B16CD - 5;
        MemUtils::WriteProcMem(0x5B16CD + 1, &fp, 4);
    }

    // Change the font sizes ~ Jason Hood. I gave up on my implementation and used his.
    {
        static DWORD fontAdjustedWidth = *screenHeight * 4 / 3;
        if (aspectRatio >= 1.6)
        {
            fontAdjustedWidth = *screenHeight * 5 / 4;
        }

        const auto adjustedWidth = &fontAdjustedWidth;
        MemUtils::WriteProcMem(0x415cc7, &adjustedWidth, 4);
        MemUtils::WriteProcMem(0x416663, &adjustedWidth, 4);
        MemUtils::WriteProcMem(0x416913, &adjustedWidth, 4);
        MemUtils::WriteProcMem(0x416be3, &adjustedWidth, 4);
        MemUtils::WriteProcMem(0x412e8d, &adjustedWidth, 4);
    }

    // Change cinematic aspect ratio
    {
        MemUtils::WriteProcMem(0x5C9020, &aspectRatio, 4);
    }

    // Adjust the nav map to use height not width
    {
        constexpr DWORD patch1 = 0x610854; // height not width
        MemUtils::WriteProcMem(0x499A3B + 1, &patch1, 4);

        // If greater than 1200 high then use NavMap1600
        constexpr DWORD patch2 = 1200; // 1600x1200
        MemUtils::WriteProcMem(0x499A43 + 1, &patch2, 4);

        // If greater than/equal 864 high then use NavMap1280 (5D3EA0)
        const std::array<byte, 10> patch3 = { 0x60, 0x03, 0x00, 0x00, 0x7C, 0x07, 0x68, 0xA0, 0x3E, 0x5D };
        MemUtils::WriteProcMem(0x499A5C + 1, patch3.data(), 10);

        // If greater/equal than 768 high then use NavMap1024 (5D3EB8)
        const std::array<byte, 17> patch4 = { 0x00, 0x03, 0x00, 0x00, 0x7C, 0x0E, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x68, 0xB8, 0x3E, 0x5D };
        MemUtils::WriteProcMem(0x499A6A + 1, patch4.data(), 17);

        // Otherwise use 800x600 (5D3EAC)
        constexpr std::array<byte, 3> patch5 = { 0xAC, 0x3E, 0x5D };
        MemUtils::WriteProcMem(0x499A7F + 1, patch5.data(), 3);
    }

    // Patch navmap to use 4/3 fov
    {
        const BYTE patch[] = { 0x68, 0x5d, 0xba, 0xd9, 0x41, 0x90, 0x90, 0x90, 0x90, 0x90 };
        MemUtils::WriteProcMem(0x495B88, &patch, 10);
    }
}

void RequiredMemEdits()
{
    const auto fl = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));
    const auto common = reinterpret_cast<DWORD>(GetModuleHandleA("common.dll"));
    const auto server = reinterpret_cast<DWORD>(GetModuleHandleA("server.dll"));
    const auto content = reinterpret_cast<DWORD>(GetModuleHandleA("content.dll"));

    createIdDetour = std::make_unique<FunctionDetour<CreateIdFunc>>(CreateID);
    createIdDetour->Detour(CreateIdDetour);

    // Patch out vanilla cursor
    MemUtils::NopAddress(0x05B1750, 4);
    MemUtils::NopAddress(0x5B32F2, 7);
    MemUtils::NopAddress(0x5B1750, 7);
    MemUtils::NopAddress(0x42025A, 13);
    MemUtils::NopAddress(0x41ECC7, 16);
    MemUtils::NopAddress(0x41EAA3, 20);

    // Patch out checks for if the cursor is visible
    MemUtils::NopAddress(0x004202F6, 7);
    MemUtils::NopAddress(0x0041EEBD, 7);
    MemUtils::NopAddress(0x00420474, 7);

    // Patch out SetCursor
    MemUtils::NopAddress(0x598989, 7);
    MemUtils::NopAddress(0x5989B6, 7);
    MemUtils::NopAddress(0x598AF2, 7);
    MemUtils::NopAddress(0x598BEC, 7);
    MemUtils::NopAddress(0x598C0B, 7);

    // Set borderless window mode
    std::array<byte, 2> borderlessPatch = { 0x00, 0x00 };
    constexpr DWORD borderlessWindowPatch1 = 0x02477A;
    constexpr DWORD borderlessWindowPatch2 = 0x02490D;
    MemUtils::WriteProcMem(fl + borderlessWindowPatch1, borderlessPatch.data(), 2);
    MemUtils::WriteProcMem(fl + borderlessWindowPatch2, borderlessPatch.data(), 2);

    // Ensure the game remains running when out of focus
    std::array<byte, 1> focusPatch = { 0xEB };
    MemUtils::WriteProcMem(fl + 0x1B2665, focusPatch.data(), focusPatch.size());

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
    MemUtils::WriteProcMem(fl + 0x0D5936, removeCruiseSpeedDisplayLimit.data(), 2);

    // Remove level requirements when buying ships and equipment
    std::array<byte, 1> removeLevelRequirement = { 0xEB };
    MemUtils::WriteProcMem(fl + 0x080499, removeLevelRequirement.data(), removeLevelRequirement.size());
    MemUtils::WriteProcMem(fl + 0x082E95, removeLevelRequirement.data(), removeLevelRequirement.size());
    MemUtils::WriteProcMem(fl + 0xB948D, removeLevelRequirement.data(), removeLevelRequirement.size());

    // Allow ESC to skip cutscenes
    std::array<byte, 3> skipStory = { 0xEB, 0x0C, 0x90 };
    MemUtils::WriteProcMem(fl + 0x5685F, skipStory.data(), skipStory.size());

    // Regenerate restart.fl on each launch and ensure that it's only loaded after regeneration
    // For some reason, this hack can cause crashes with an invalid pointer.
    // We fix this by creating a static pointer to the string compare function, then writing the bytes into the hack dynamically.
    // ReSharper disable once CppDeprecatedEntity
    static auto cmp = reinterpret_cast<PDWORD>(_strcmpi); // NOLINT(clang-diagnostic-deprecated-declarations)
    const auto ptr = &cmp;
    std::array<byte, 45> regenerateRestartFl = { 0x8D, 0x8C, 0x24, 0x5C, 0x01, 0x00, 0x00, 0x51, 0x8D, 0x54, 0x24, 0x5C, 0x52, 0xEB, 0x13,
                                                 0xFF, 0x11, 0x83, 0xC4, 0x08, 0x85, 0xC0, 0x74, 0x11, 0x8B, 0xCD, 0xE8, 0x22, 0xFD, 0xFF,
                                                 0xFF, 0xEB, 0x0F, 0x90, 0xB9, 0x48, 0x4A, 0xD6, 0x06, 0xEB, 0xE6, 0x83, 0xC4, 0x08, 0xEB };
    std::memcpy(regenerateRestartFl.data() + 35, &ptr, 4);

    MemUtils::WriteProcMem(server + 0x6900F, regenerateRestartFl.data(), regenerateRestartFl.size());

    // disable PlayerEnemyClamp altogether; instead making NPC enemy target selection random.
    std::array<byte, 2> disableNpcClamp = { 0xEB, 0x39 };
    MemUtils::WriteProcMem(common + 0x08E86A, disableNpcClamp.data(), disableNpcClamp.size());

    // NPCs use scanner (enables CMs).
    std::array<byte, 1> useScanner = { 0x00 };
    MemUtils::WriteProcMem(common + 0x013E52C, useScanner.data(), useScanner.size());

    // Allow the fc_n_grp faction to drop [phantom_loot] loot..
    std::array<byte, 1> nomadPhantom = { 0x01 };
    MemUtils::WriteProcMem(content + 0x0BD2D8, nomadPhantom.data(), nomadPhantom.size());

    //  Adjust cruise speed according to drag_modifier.
    std::array<byte, 1> cruiseDragModifier = { 0xEB };
    MemUtils::WriteProcMem(common + 0x053796, cruiseDragModifier.data(), cruiseDragModifier.size());

    // Enable thruster sounds when going backwards
    std::array<byte, 6> thrusterSounds1 = { 0xD9, 0xE1, 0xD9, 0x5C, 0xE4, 0x08 };
    MemUtils::WriteProcMem(fl + 0x012F217, thrusterSounds1.data(), thrusterSounds1.size());

    std::array<byte, 1> thrusterSounds2 = { 0x00 };
    MemUtils::WriteProcMem(fl + 0x012F221, thrusterSounds2.data(), thrusterSounds2.size());

    // Ensure that drag modifier works even if inside
    std::array<byte, 2> dragModifierAllZone = { 0x41, 0x74 };
    MemUtils::WriteProcMem(common + 0x0DAD24, dragModifierAllZone.data(), dragModifierAllZone.size());

    // Allow the fc_n_grp faction to drop [phantom_loot] loot..
    int missileFlag = 15;
    MemUtils::WriteProcMem(content + 0x0F20F0, &missileFlag, sizeof(int));

    // Fix sounds not playing on negative thrust
    std::array<byte, 6> soundFix1 = { 0xD9, 0xE1, 0xD9, 0x5C, 0xe4, 0x08 };
    std::array<byte, 1> soundFix2 = { 0x00 };
    MemUtils::WriteProcMem(fl + 0x12F217, soundFix1.data(), soundFix1.size());
    MemUtils::WriteProcMem(fl + 0x12F221, soundFix2.data(), soundFix2.size());

    // Allow the player to form on hostiles
    float newFormationMin = -2.0f;
    MemUtils::WriteProcMem(common + 0x13F540, &newFormationMin, sizeof(float));

    std::array<byte, 26> uiSmoothness = { 0x60, 0x8B, 0x86, 0x85, 0x03, 0x00, 0x00, 0x8D, 0x9E, 0x5E, 0x03, 0x00, 0x00,
                                          0x50, 0x53, 0xE8, 0x37, 0x6C, 0xFD, 0xFF, 0x83, 0xC4, 0x08, 0x61, 0xEB, 0x63 };
    MemUtils::WriteProcMem(fl + 0x18B2D5, uiSmoothness.data(), uiSmoothness.size());

    // Comm/Background chatter play immediately
    MemUtils::NopAddress(fl + 0xCDB41, 2);
    MemUtils::NopAddress(fl + 0xCE71E, 2);

    // Patch HP_Fire extra audio not being played
    std::array<byte, 9> hpFirePatch = { 0x83, 0xFA, 0xFF, 0xBA, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3 };
    MemUtils::WriteProcMem(common + 0x039F77, hpFirePatch.data(), hpFirePatch.size());

    // Replace the word 'MULTIPLAYER' with 'CHAOS MOD' in the options menu
    uint menu3Ids = 458753;
    MemUtils::WriteProcMem(fl + 0xAABFC, &menu3Ids, 4);

    BugFixes::SetupDetours();

    PatchResolution();
}

void* ChaosMod::ScriptLoadHook(const char* script)
{
    DelayedInit();
    Get<UiManager>()->SetCursor("arrow");

    thornLoadDetour->UnDetour();
    return thornLoadDetour->GetOriginalFunc()(script);
}

void Update(const double delta)
{
    timeCounter += delta;
    while (timeCounter > SixtyFramesPerSecond)
    {
        if (!OffsetHelper::IsGamePaused())
        {
            SaveManager::SaveTimer(SixtyFramesPerSecond);
            Get<ChaosTimer>()->Update(SixtyFramesPerSecond);
            Get<GlobalTimers>()->Update(SixtyFramesPerSecond);
        }

        Get<DiscordManager>()->Update();

        timeCounter -= SixtyFramesPerSecond;
    }

    if (!OffsetHelper::IsGamePaused())
    {
        Get<ChaosTimer>()->FrameUpdate(static_cast<float>(delta));
        PatchNotes::Update(static_cast<float>(delta));
    }

    Get<TwitchVoting>()->Poll();

    timingDetour->UnDetour();
    timingDetour->GetOriginalFunc()(delta);
    timingDetour->Detour(Update);
}

void CreateDefaultPerfOptions()
{
    char path[MAX_PATH];
    GetUserDataPath(path);
    const std::string filePath = std::format("{}/PerfOptions.ini", path);
    if (std::filesystem::exists(filePath))
    {
        return;
    }

    const auto str = GetResourceString(Utils::ResourceIds::DefaultPerfOptions);
    if (str.empty())
    {
        return;
    }

    std::ofstream file(filePath, std::ios::beg | std::ios::trunc);
    file << str << std::endl;
}

FunctionDetour freeLibraryDetour(FreeLibrary);
ChaosMod::ChaosMod()
{
    freeLibraryDetour.Detour(FreeLibraryDetour);

    // The very first thing we do is change the saved data folder so we can save and load properly
    std::string newSavedDataFolder = "FLChaosMod";
    MemUtils::WriteProcMem(reinterpret_cast<DWORD>(GetModuleHandleA("common.dll")) + 0x142684, newSavedDataFolder.data(), newSavedDataFolder.length());

    CreateDefaultPerfOptions();

    AssetTracker::StartDetours();
    OnHit::Detour();

    // make needed memory edits for chaos mod to work
    RequiredMemEdits();

    // Register only the components that have to be done now
    SetComponent<Random>(); // Almost everything depends on Random
    SetComponent<GlobalTimers>();

    Get<SplashScreen>()->SetLoadingMessage(5);
    ConfigManager::Load();

    Get<SplashScreen>()->SetLoadingMessage(10);
    SetComponent<ChaosTimer>();
    SetComponent<DrawingHelper>();
    Get<SplashScreen>()->SetLoadingMessage(15);
    SetComponent<UiManager>();
    SetComponent<KeyManager>();
    SetComponent<ReshadeManager>();
    SetComponent<DiscordManager>();

    // Setup hooks
    const HMODULE common = GetModuleHandleA("common");
    const auto fl = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));
    timingDetour = std::make_unique<FunctionDetour<GlobalTimeFunc>>(
        reinterpret_cast<GlobalTimeFunc>(fl + 0x1B2890)); // NOLINT
    thornLoadDetour = std::make_unique<FunctionDetour<ScriptLoadPtr>>(
        reinterpret_cast<ScriptLoadPtr>(GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z"))); // NOLINT

    timingDetour->Detour(Update);
    thornLoadDetour->Detour(ScriptLoadHook);
}

HMODULE dll;
void __stdcall ChaosMod::TerminateAllThreads()
{
    for (const auto& possibleEffects = ActiveEffect::GetAllEffects(); const auto& effect : possibleEffects)
    {
        effect->Cleanup();
    }

    // Explicitly reset any components that make use of threads
    ResetComponent<MoviePlayer>();

    FreeLibraryAndExitThread(dll, 0);
}

BOOL __stdcall ChaosMod::FreeLibraryDetour(const HMODULE handle)
{
    if (dll == handle)
    {
        freeLibraryDetour.UnDetour();
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(TerminateAllThreads), nullptr, 0, nullptr);

        return true;
    }

    freeLibraryDetour.UnDetour();
    const auto res = FreeLibrary(handle);
    freeLibraryDetour.Detour(FreeLibraryDetour);
    return res;
}

extern "C" __declspec(dllexport) void Dummy() {}

FunctionDetour startUpDetour(DALib::Startup);

bool OnStartUp(HWND window, const char* unk)
{
    float newX = 0.5f;
    float newY = 0.5f;

    MemUtils::WriteProcMem(0x4dd493, &newX, sizeof(float));
    MemUtils::WriteProcMem(0x4dd49b, &newY, sizeof(float));

    // Display loading screen while we load all our chaos content
    // It takes a lot longer than vanilla!
    SetComponent<SplashScreen>(window);
    Get<SplashScreen>()->SetLoadingMessage(2);

    // Force construct ChaosMod
    ChaosMod::i();
    Get<ReshadeManager>()->SetHModule(dll);

    startUpDetour.UnDetour();
    return DALib::Startup(window, unk);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    DisableThreadLibraryCalls(hModule);
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        dll = hModule;

        // Defer our start up so we bypass the DLL loader lock
        // We need our threads :)
        startUpDetour.Detour(OnStartUp);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        reshade::unregister_addon(hModule);
    }
    return TRUE;
}
