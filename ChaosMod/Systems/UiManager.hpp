#pragma once

#include "ImguiComponents/ActiveEffectsText.hpp"
#include "ImguiComponents/ChaosOptionText.hpp"
#include "d3d9.h"

#include "ImguiComponents/Debug.hpp"
#include "ImguiComponents/ProgressBar.hpp"

class UiManager final : public Singleton<UiManager>
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

        static void LoadWinKey();

        ChaosOptionText optionText;
        DebugMenu debugLog;
        ProgressBar progressBar;
        ActiveEffectsText activeEffectsText;

    public:
        void SetCursor(std::string str);

        UiManager();
        ~UiManager();
        void LoadCursors();

        static void Setup(LPDIRECT3DDEVICE9 device, HWND window);

        void Render();

        void ToggleDebugConsole();
        void DebugLog(const std::string& log);

        void UpdateProgressBar(float progressPercentage);

        enum class Font
        {
            TitiliumWeb,
            TitiliumWebLarge,
        };

    private:
        static std::map<Font, ImFont*> loadedFonts;

    public:
        static ImFont* GetFont(Font font);
};
