#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
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
            const auto drawing = Get<DrawingHelper>();
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
                        nextTimestamp = nextTimestamp = Get<Random>()->UniformFloat(0.5f, 1.5f);
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
                            nextTimestamp = Get<Random>()->UniformFloat(2.5f, 5.0f);
                        }
                    }
                    break;
                case TiredMode::Waiting:
                    if (nextTimestamp < 0.0f)
                    {
                        direction.x = Get<Random>()->UniformFloat(-1.0f, 1.0f);
                        direction.y = Get<Random>()->UniformFloat(-1.0f, 1.0f);
                        currentMode = TiredMode::Closing;
                    }
                    break;
            }

            const auto behaviour = ship->get_behavior_interface();
            if (const auto op = behaviour->get_current_directive()->op;
                alpha > 200 && (op == pub::AI::FreeFlight || op == pub::AI::Goto || op == pub::AI::None))
            {
                // As the eyes start to close start turning the players ship
                Vector* steering = reinterpret_cast<Vector*>(reinterpret_cast<const PCHAR>(behaviour) + 292);
                steering->x = direction.x;
                steering->y = direction.y;
            }

            BlackOut(alpha);
        }

    public:
        explicit SleepyPlayer(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SleepyPlayer, {
    .effectName = "Sleepy Player",
    .description = "Aww, you poor thing. Didn't get enough sleep? You're falling asleep at the joystick.",
    .category = EffectType::Visual
});
