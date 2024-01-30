#pragma once

#include "d3d9.h"
#include "imgui/imgui_internal.h"

class UiManager final : public Singleton<UiManager>
{
        std::optional<HCURSOR> cursorOverride;

        struct HkMouseState
        {
                uint leftDown;
                uint rightDown;
                uint middleDown;
                uint mouse4Down;
                uint mouse5Down;
        };

        ImGuiContext* context;
        HWND window = nullptr;
        std::map<std::string, HCURSOR> mapCursors;

        static LRESULT __stdcall WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
        static HkMouseState ConvertState(DWORD state);
        void HandleInput();
        static bool WinKeyDetour(uint msg, WPARAM wParam, LPARAM lParam);
        static bool OnCursorChangeDetour(const char* cursorName, bool hideCursor);

    public:
        void OverrideCursor(std::optional<HCURSOR> cursor);
        void SetCursor(std::string str);

        UiManager();
        ~UiManager();
        void LoadCursors();

        static void Setup(LPDIRECT3DDEVICE9 device, HWND window);

        void Render();
};
