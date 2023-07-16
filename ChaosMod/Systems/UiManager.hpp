#pragma once

#include "d3d9.h"

#include "ImGuiDX9.hpp"
#include "ImguiComponents/Debug.hpp"

class UiManager : public Singleton<UiManager>
{
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

        static LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static HkMouseState ConvertState(DWORD state);
        void HandleInput();
        static bool WinKeyDetour(uint msg, WPARAM wParam, LPARAM lParam);

        void PatchShowCursor() const;
        static void LoadWinKey();

        struct SelectionText
        {
                float progress = 0.0f;
                bool show = true;
                void Render();
        } selectionText;

        DebugLog debugLog;

    public:
        void SetCursor(std::string str);

        UiManager();
        ~UiManager();
        void LoadCursors();

        static void Setup(LPDIRECT3DDEVICE9 device, HWND window);
        void Render();

        void ToggleDebugConsole();
        void DebugLog(const std::string& log);
};
