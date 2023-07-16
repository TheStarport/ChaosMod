#include "PCH.hpp"

#include "DirectX/d3d.hpp"
#include "UiManager.hpp"

#include "ImGuiHelpers/ImGuiDX9.hpp"
#include "ImGuiHelpers/ImGuiWin32.hpp"

#include <iostream>
#include <shellapi.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

typedef int(__stdcall* OriginalWndProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
OriginalWndProc originalProc;
PBYTE originalProcData = PBYTE(malloc(5));

LRESULT CALLBACK UiManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    i()->window = hWnd;

    const HCURSOR arrowCursor = LoadCursor(NULL, IDC_ARROW);
    CURSORINFO ci;
    ci.cbSize = sizeof(ci);
    if (GetCursorInfo(&ci) && ci.hCursor == arrowCursor)
    {
        i()->SetCursor("arrow");
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    Utils::Memory::UnDetour(PBYTE(originalProc), originalProcData);
    const LRESULT result = originalProc(hWnd, msg, wParam, lParam);
    Utils::Memory::Detour(PBYTE(originalProc), WndProc, originalProcData);
    return result;
}

DWORD* mouseX = reinterpret_cast<PDWORD>(0x616840);
DWORD* mouseY = reinterpret_cast<PDWORD>(0x616844);
DWORD* mouseStateRaw = reinterpret_cast<PDWORD>(0x616850);

UiManager::HkMouseState UiManager::ConvertState(const DWORD state)
{
    return {
        (state & 1), (state & 2), (state & 4), (state & 8), (state & 16),
    };
}

void UiManager::HandleInput()
{
    constexpr int left = 0, right = 1, middle = 2, x1 = 3, x2 = 4;

    ImGuiIO& io = ImGui::GetIO();

    const DWORD xPos = *mouseX;
    const DWORD yPos = *mouseY;

    const auto [leftDown, rightDown, middleDown, mouse4Down, mouse5Down] = ConvertState(*mouseStateRaw);

    // Position before anything else
    io.AddMousePosEvent(static_cast<float>(xPos), static_cast<float>(yPos));
    io.AddMouseButtonEvent(left, leftDown);
    io.AddMouseButtonEvent(right, rightDown);
    io.AddMouseButtonEvent(middle, middleDown);
    io.AddMouseButtonEvent(x1, mouse4Down);
    io.AddMouseButtonEvent(x2, mouse5Down);

    if (!debugLog.show && GetAsyncKeyState(VK_F5))
    {
        ToggleDebugConsole();
    }
}

void* WINAPI CreateDirect3D8(UINT SDKVersion)
{
    char path[MAX_PATH];
    GetSystemDirectoryA(path, MAX_PATH);
    strcat_s(path, MAX_PATH, "\\d3d8.dll");

    /*HMODULE d3ddll = LoadLibraryA(Path);

    typedef IDirect3D9*(WINAPI * D3DProc8)(UINT);
    D3DProc8 func = (D3DProc8)GetProcAddress(d3ddll, "Direct3DCreate8");*/

    IDirect3D9* const d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (d3d == nullptr)
    {
        return nullptr;
    }

    if (const HMODULE module = LoadLibrary(TEXT("d3dx9_43.dll")); module == nullptr)
    {
        if (MessageBoxA(nullptr,
                        "Failed to load d3dx9_43.dll! Some features will not work correctly.\n\n"
                        "It's required to install the \"Microsoft DirectX End-User Runtime\" in order to use directx.\n\n"
                        "Please click \"OK\" to open the official download page or \"CANCEL\" to continue anyway.",
                        nullptr,
                        MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND | MB_OKCANCEL | MB_DEFBUTTON1) == IDOK)
        {
            // Open the url
            ShellExecuteA(nullptr, "open", "https://www.microsoft.com/download/details.aspx?id=35", nullptr, nullptr, SW_SHOW);
            return nullptr;
        }
    }

    return new Direct3D8(d3d);
}

typedef bool(__cdecl* OnCursorChange)(const char* cursorName, bool hideCursor);
OnCursorChange onCursorChange;
PBYTE onCursorChangeData = PBYTE(malloc(5));

std::string curCursor;

bool OnCursorChangeDetour(const char* cursorName, bool hideCursor)
{
    std::string name = cursorName;
    if (name == "Cross")
    {
        name = "cross";
    }

    else if (name == "Arrow")
    {
        name = "arrow";
    }

    curCursor = name;
    UiManager::i()->SetCursor(name);
    return true;
}

void UiManager::SetCursor(const std::string str)
{
    if (const auto cur = this->mapCursors.find(str); cur != this->mapCursors.end())
    {
        ::SetCursor(cur->second);
        // SetClassLongPtrA(UI::window, GCLP_HCURSOR, (LONG_PTR)cur->second);
        PostMessage(window, WM_SETCURSOR, static_cast<WPARAM>(1), reinterpret_cast<LPARAM>(cur->second));
    }
}

void UiManager::PatchShowCursor() const
{
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
}

PBYTE winKeyOriginalMem;
typedef bool(__cdecl* WinKeyType)(uint msg, WPARAM wParam, LPARAM lParam);
WinKeyType winKeyOriginal;

bool UiManager::WinKeyDetour(uint msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_KEYUP:
        case WM_KEYDOWN:
            {
                if (ImGui::GetIO().WantCaptureKeyboard)
                {
                    return false;
                }
            }
        default: break;
    }
    Utils::Memory::UnDetour(PBYTE(winKeyOriginal), winKeyOriginalMem);
    auto result = winKeyOriginal(msg, wParam, lParam);
    Utils::Memory::Detour(PBYTE(winKeyOriginal), WinKeyDetour, winKeyOriginalMem);
    return result;
}

void UiManager::LoadWinKey()
{
    winKeyOriginalMem = PBYTE(malloc(5));
    winKeyOriginal = WinKeyType(0x577850);
    Utils::Memory::Detour(PBYTE(winKeyOriginal), WinKeyDetour, winKeyOriginalMem);
}

UiManager::UiManager()
{
    const auto fl = reinterpret_cast<char*>(GetModuleHandle(nullptr));

    IMGUI_CHECKVERSION();
    context = ImGui::CreateContext();

    const ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    const auto rp8 = reinterpret_cast<char*>(LoadLibrary(L"RP8.dll"));
    char* pAddress = rp8 + 0x5E188;
    auto fpD3D8CreateHook = reinterpret_cast<FARPROC>(CreateDirect3D8);
    Utils::Memory::WriteProcMem(pAddress, &fpD3D8CreateHook, 4);

    // Disable Freelancer's vanilla cursor and restore the windows version
    PatchShowCursor();

    onCursorChange = reinterpret_cast<OnCursorChange>(0x41DDE0);
    Utils::Memory::Detour(reinterpret_cast<PBYTE>(onCursorChange), OnCursorChangeDetour, onCursorChangeData);

    // Set borderless window mode
    BYTE patch[] = { 0x00, 0x00 };
    constexpr DWORD BorderlessWindowPatch1 = 0x02477A;
    constexpr DWORD BorderlessWindowPatch2 = 0x02490D;
    Utils::Memory::WriteProcMem(fl + BorderlessWindowPatch1, &patch, 2);
    Utils::Memory::WriteProcMem(fl + BorderlessWindowPatch2, &patch, 2);

    originalProc = OriginalWndProc(0x5B2570);
    Utils::Memory::Detour(PBYTE(originalProc), WndProc, originalProcData);

    LoadCursors();
    LoadWinKey();
}

UiManager::~UiManager()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX9_Shutdown();
    ImGui::DestroyContext(context);
}

void UiManager::LoadCursors()
{
    try
    {
        char szCurDir[MAX_PATH];
        GetCurrentDirectoryA(sizeof(szCurDir), szCurDir);
        std::string dir = std::string(szCurDir);

        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir + "/../DATA/CURSORS"))
        {
            if (!entry.is_regular_file() || entry.file_size() > UINT_MAX || !Utils::String::EndsWith(entry.path().generic_string(), ".cur"))
            {
                continue;
            }

            const HCURSOR hCur = LoadCursorFromFileA(entry.path().generic_string().c_str());
            if (!hCur)
            {
                continue;
            }

            if (this->mapCursors.contains(entry.path().stem().generic_string()))
            {
                continue;
            }

            this->mapCursors[entry.path().stem().generic_string()] = hCur;
        }
    }
    catch (std::exception& ex)
    {
        std::string what = ex.what();
        DebugLog(what);
    }
}

void UiManager::Setup(const LPDIRECT3DDEVICE9 device, const HWND window)
{
    i()->window = window;
    ImGui_ImplDX9_Init(device);
    ImGui_ImplWin32_Init(window);

    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
}

void UiManager::Render()
{
    HandleInput();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    debugLog.Render();
    optionText.Render();
    progressBar.Render();

    ImGui::ShowDemoWindow();

    ImGui::EndFrame();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void UiManager::ToggleDebugConsole() { debugLog.show = !debugLog.show; }

void UiManager::DebugLog(const std::string& log) { debugLog.Log(log); }
void UiManager::UpdateProgressBar(const float progressPercentage) { progressBar.SetProgressPercentage(progressPercentage); }
