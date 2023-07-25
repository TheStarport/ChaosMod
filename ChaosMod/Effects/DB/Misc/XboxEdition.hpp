#pragma once
#include "Effects/ActiveEffect.hpp"

class XboxEdition final : public ActiveEffect
{
        std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

        // Cap the FPS
        constexpr static float fpsLimit = (1.0f / 20.0f) * 1000.0f;
        void FrameUpdate(float delta) override
        {
            a = std::chrono::system_clock::now();

            if (const std::chrono::duration<float, std::milli> duration = a - b; duration.count() < fpsLimit)
            {
                const std::chrono::duration<float, std::milli> deltaMs(fpsLimit - duration.count());
                const auto deltaMsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(deltaMs);
                std::this_thread::sleep_for(std::chrono::milliseconds(deltaMsDuration.count()));
            }

            b = std::chrono::system_clock::now();
        }

    public:
        DefEffectInfo("Xbox Edition", 1.0f, EffectType::Misc);
};
