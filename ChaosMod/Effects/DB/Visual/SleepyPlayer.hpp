#pragma once
#include "Systems/DirectX/Drawing.hpp"

class SleepyPlayer final : public ActiveEffect
{
        enum class TiredMode
        {
            Closing,
            Opening,
            Waiting,
        };

        TiredMode currentMode = TiredMode::Closing;
        int alpha = 0;
        float nextTimestamp = 0.0f;
        int closingIterator = 0;
        glm::vec2 direction{};

        void BlackOut(const int alpha)
        {
            const auto drawing = DrawingHelper::i();
            drawing->BoxFilled(0.f, 0.0f, 1.f, 1.f, D3DCOLOR_ARGB(alpha, 0, 0, 0));
            if (const float progress = static_cast<float>(alpha) / 255.f; progress > 0)
            {
                drawing->BoxFilled(0.f, 0.f, 1, progress / 2, D3DCOLOR_XRGB(0, 0, 0));                  // top bar
                drawing->BoxFilled(0.f, 1.f - (progress / 4), 1, progress / 2, D3DCOLOR_XRGB(0, 0, 0)); // bottom bar
            }
        }

        void Begin() override
        {
            currentMode = TiredMode::Closing;
            alpha = 0;
            closingIterator = 1;
        }

        void FrameUpdate(float delta) override
        {
            const auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            nextTimestamp -= delta;
            switch (currentMode)
            {
                case TiredMode::Closing:
                    alpha += closingIterator;

                    if (alpha >= 255)
                    {
                        currentMode = TiredMode::Opening;
                        nextTimestamp = nextTimestamp = Random::i()->UniformFloat(0.5f, 1.5f);
                    }
                    break;
                case TiredMode::Opening:
                    if (nextTimestamp < 0.0f)
                    {
                        nextTimestamp = 0.05f;
                        alpha -= 30;
                        if (alpha <= 0)
                        {
                            alpha = 0;
                            currentMode = TiredMode::Waiting;
                            nextTimestamp = Random::i()->UniformFloat(2.5f, 5.0f);
                        }
                    }
                    break;
                case TiredMode::Waiting:
                    if (nextTimestamp < 0.0f)
                    {
                        direction.x = Random::i()->UniformFloat(-1.0f, 1.0f);
                        direction.y = Random::i()->UniformFloat(-1.0f, 1.0f);
                        currentMode = TiredMode::Closing;
                    }
                    break;
            }

            if (alpha > 200)
            {
                // As the eyes start to close start turning the players ship
                Vector* steering = reinterpret_cast<Vector*>(reinterpret_cast<PCHAR>(ship->get_behavior_interface()) + 292);
                steering->x = direction.x;
                steering->y = direction.y;
            }

            BlackOut(alpha);
        }

    public:
        DefaultEffectInfo("Sleepy Player", EffectType::Visual);
};
