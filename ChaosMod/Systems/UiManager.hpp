#pragma once

#include "d3d9.h"

#include "ImGuiDX9.hpp"

class UiManager : public Singleton<UiManager>
{
        struct HkMouseState
        {
                int LeftDown;
                int RightDown;
                int MiddleDown;
                int Mouse4Down;
                int Mouse5Down;
        };

        ImGuiContext* context;
        HWND hwnd{};
        std::map<std::string, HCURSOR> mapCursors;

        static LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static HkMouseState ConvertState(DWORD state);
        void HandleInput();
        static bool WinKeyDetour(uint msg, WPARAM wParam, LPARAM lParam);

        void PatchShowCursor() const;
        static void LoadWinKey();

        struct DebugLog
        {
                DebugLog() { lineOffsets.push_back(0); }

                ImGuiTextBuffer buf;
                ImGuiTextFilter filter;
                ImVector<int> lineOffsets;

                bool show = true;
                void Render();

        } debugLogger;

        struct SelectionText
        {
                float progress = 0.0f;
                bool show = true;
                void Render();
        } selectionText;

    public:
        void SetCursor(std::string str);

        UiManager();
        ~UiManager();
        void LoadCursors();

        static void Setup(const LPDIRECT3DDEVICE9 device, const HWND hwnd);
        void Render();

        void ToggleDebugConsole();
        void DebugLog(std::string& log);
};
