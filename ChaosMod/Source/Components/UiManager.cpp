#include "PCH.hpp"

#include "Components/UiManager.hpp"
#include "DirectX/d3d.hpp"

#include "Components/ConfigManager.hpp"
#include "ImGui/Helpers/ImGuiWin32.hpp"
#include "imgui_internal.h"

#include <iostream>
#include <shellapi.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// Very hacky way to stop reshade from polluting ImGui
#define RESHADE_API_LIBRARY_EXPORT
#include "Components/ReshadeManager.hpp"
#include "Components/SplashScreen.hpp"
#include "ImGui/Helpers/ImGuiDX9.hpp"
#include "ImGui/ImGuiManager.hpp"
#undef RESHADE_API_LIBRARY_EXPORT

typedef LRESULT(__stdcall* OriginalWndProc)(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
FunctionDetour wndProcDetour(reinterpret_cast<OriginalWndProc>(0x5B2570));

LRESULT __stdcall UiManager::WndProc(const HWND hWnd, const uint msg, const WPARAM wParam, const LPARAM lParam)
{
    Get<UiManager>()->window = hWnd;

    const HCURSOR arrowCursor = LoadCursor(NULL, IDC_ARROW);
    CURSORINFO ci;
    ci.cbSize = sizeof(ci);
    if (GetCursorInfo(&ci) && ci.hCursor == arrowCursor)
    {
        Get<UiManager>()->SetCursor("arrow");
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    wndProcDetour.UnDetour();
    const LRESULT result = wndProcDetour.GetOriginalFunc()(hWnd, msg, wParam, lParam);
    wndProcDetour.Detour(WndProc);
    return result;
}

DWORD* mouseX = reinterpret_cast<PDWORD>(0x616840);
DWORD* mouseY = reinterpret_cast<PDWORD>(0x616844);
DWORD* mouseStateRaw = reinterpret_cast<PDWORD>(0x616850);
int* mouseZ = reinterpret_cast<int*>(0x616848);

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
    const int scroll = *mouseZ;

    const auto [leftDown, rightDown, middleDown, mouse4Down, mouse5Down] = ConvertState(*mouseStateRaw);

    // Position before anything else
    io.AddMousePosEvent(static_cast<float>(xPos), static_cast<float>(yPos));
    io.AddMouseButtonEvent(left, leftDown);
    io.AddMouseButtonEvent(right, rightDown);
    io.AddMouseButtonEvent(middle, middleDown);
    io.AddMouseButtonEvent(x1, mouse4Down);
    io.AddMouseButtonEvent(x2, mouse5Down);
    if (scroll)
    {
        io.AddMouseWheelEvent(0.f, static_cast<float>(scroll));
        *mouseZ = 0;
    }
}

void* WINAPI CreateDirect3D8(uint SDKVersion)
{
    char path[MAX_PATH];
    GetSystemDirectoryA(path, MAX_PATH);
    strcat_s(path, MAX_PATH, "\\d3d8.dll");

    /*HMODULE d3ddll = LoadLibraryA(Path);

    typedef IDirect3D9*(WINAPI * D3DProc8)(uint);
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

    Get<ReshadeManager>()->InitReshade();

    Get<SplashScreen>()->SetLoadingMessage(55);
    return new Direct3D8(d3d);
}

typedef bool(__cdecl* OnCursorChange)(const char* cursorName, bool hideCursor);
FunctionDetour cursorDetour(reinterpret_cast<OnCursorChange>(0x41DDE0));

std::string curCursor;

bool UiManager::OnCursorChangeDetour(const char* cursorName, bool hideCursor)
{
    if (HCURSOR override = Get<UiManager>()->cursorOverride.value_or(nullptr))
    {
        ::SetCursor(override);
        PostMessage(Get<UiManager>()->window, WM_SETCURSOR, static_cast<WPARAM>(1), reinterpret_cast<LPARAM>(override));
        return true;
    }

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
    Get<UiManager>()->SetCursor(name);
    return true;
}

void UiManager::OverrideCursor(const std::optional<HCURSOR> cursor)
{
    cursorOverride = cursor;
    if (cursor.has_value())
    {
        ::SetCursor(cursor.value());
        PostMessage(Get<UiManager>()->window, WM_SETCURSOR, static_cast<WPARAM>(1), reinterpret_cast<LPARAM>(cursor.value()));
    }
}

void UiManager::SetCursor(const std::string& str)
{
    if (const auto cur = this->mapCursors.find(str); cur != this->mapCursors.end())
    {
        ::SetCursor(cur->second);
        // SetClassLongPtrA(UI::window, GCLP_HCURSOR, (LONG_PTR)cur->second);
        PostMessage(window, WM_SETCURSOR, static_cast<WPARAM>(1), reinterpret_cast<LPARAM>(cur->second));
    }
}

typedef bool(__cdecl* WinKeyType)(uint msg, WPARAM wParam, LPARAM lParam);
FunctionDetour winKeyDetour(reinterpret_cast<WinKeyType>(0x577850));

bool UiManager::WinKeyDetour(const uint msg, const WPARAM wParam, const LPARAM lParam)
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

    winKeyDetour.UnDetour();
    const auto result = winKeyDetour.GetOriginalFunc()(msg, wParam, lParam);
    winKeyDetour.Detour(WinKeyDetour);

    return result;
}

UiManager::UiManager()
{
    IMGUI_CHECKVERSION();
    context = ImGui::CreateContext();

    const ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    const auto rp8 = reinterpret_cast<DWORD>(LoadLibrary(L"RP8.dll"));
    const auto address = rp8 + 0x5E188;
    const auto d3D8CreateHook = reinterpret_cast<FARPROC>(CreateDirect3D8);
    MemUtils::WriteProcMem(address, &d3D8CreateHook, 4);

    winKeyDetour.Detour(WinKeyDetour);
    cursorDetour.Detour(OnCursorChangeDetour);
    wndProcDetour.Detour(WndProc);

    LoadCursors();
}

UiManager::~UiManager()
{
    ImGui_ImplWin32_Shutdown();

    // Clear buffer
    auto io = ImGui::GetIO();
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
    IM_DELETE(io.BackendRendererUserData);

    ImGui::DestroyContext(context);
}

void UiManager::LoadCursors()
{
    try
    {
        char szCurDir[MAX_PATH];
        GetCurrentDirectoryA(sizeof(szCurDir), szCurDir);

        for (const auto dir = std::string(szCurDir); const auto& entry : std::filesystem::recursive_directory_iterator(dir + "/../DATA/CHAOS/VANILLA_CURSORS"))
        {
            if (!entry.is_regular_file() || entry.file_size() > UINT_MAX || !(entry.path().generic_string().ends_with(".cur")))
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
        const std::string what = ex.what();
        Log(what);
    }
}

void UiManager::Setup(const LPDIRECT3DDEVICE9 device, const HWND window)
{
    Get<SplashScreen>()->SetLoadingMessage(80);
    Get<UiManager>()->window = window;
    ImGui_ImplDX9_Init(device);
    ImGui_ImplWin32_Init(window);

    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

    char path[MAX_PATH];
    GetUserDataPath(path);

    auto& io = ImGui::GetIO();

    const static std::string ini = std::format("{}/imgui.ini", std::string(path));
    const static std::string log = std::format("{}/imgui.log", std::string(path));
    io.IniFilename = ini.c_str();
    io.LogFilename = log.c_str();

    ImGuiManager::Init();

    // Use a more orange theme
    auto& style = ImGui::GetStyle();
    /*ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.91f, 0.59f, 0.07f, 0.73f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.91f, 0.59f, 0.07f, 0.63f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.91f, 0.59f, 0.07f, 0.61f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.18f, 0.73f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.83f, 0.83f, 0.83f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);*/

    style.PopupBorderSize = 0;
    style.WindowBorderSize = 0;
    style.ChildBorderSize = 0;
    style.FrameBorderSize = 0;

    style.ScrollbarRounding = 12;
    style.WindowRounding = 6;
}

void UiManager::Render()
{
    HandleInput();

    ImGuiManager::Render();
}
