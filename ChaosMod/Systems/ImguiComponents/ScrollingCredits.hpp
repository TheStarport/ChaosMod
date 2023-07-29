#pragma once

class ScrollingCredits final
{
        inline static bool show = false;
        inline static float scrollAmount = 0.0f;
        std::chrono::duration<long long, std::ratio<1, 1000000000>> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
        std::vector<std::string> credits;

    public:
        ScrollingCredits();
        static void ToggleVisibility();
        void Render();
};
