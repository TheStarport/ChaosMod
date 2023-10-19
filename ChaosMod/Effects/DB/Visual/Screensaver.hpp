#pragma once

#include "Systems/DirectX/Drawing.hpp"

class Screensaver final : public ActiveEffect
{
        glm::vec2 resolution = {};
        float speedX = 75.0f;
        float speedY = 75.0f;
        float sizeX = 0.0f;
        float sizeY = 0.0f;

        glm::vec2 pos{};

        void Begin() override
        {
            resolution = DrawingHelper::i()->GetResolution();

            pos = resolution * 0.5f;
            speedX *= Random::i()->Uniform(0u, 1u) ? -1 : 1;
            speedY *= Random::i()->Uniform(0u, 1u) ? -1 : 1;

            sizeX = 40.0f / 100 * resolution.x;
            sizeY = 40.0f / 100 * resolution.y;
        }

        void FrameUpdate(float delta) override
        {
            if (pos.x + sizeX >= resolution.x || pos.x <= 0)
            {
                speedX *= -1;
            }

            if (pos.y + sizeY >= resolution.y || pos.y <= 0)
            {
                speedY *= -1;
            }

            pos.x += speedX * delta;
            pos.y += speedY * delta;

            auto rel = pos;
            rel.x /= resolution.x;
            rel.y /= resolution.y;

            DrawingHelper::i()->CaptureSurface(rel.x, rel.y, .4f, 0.4f);
        }

    public:
        DefaultEffectInfo("Screensaver", EffectType::Visual);
};
