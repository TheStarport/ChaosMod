#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "DirectX/Drawing.hpp"

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
            resolution = Get<DrawingHelper>()->GetResolution();

            pos = resolution * 0.5f;
            speedX *= Get<Random>()->Uniform(0u, 1u) ? -1 : 1;
            speedY *= Get<Random>()->Uniform(0u, 1u) ? -1 : 1;

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

            Get<DrawingHelper>()->CaptureSurface(rel.x, rel.y, .4f, 0.4f);
        }

    public:
        explicit Screensaver(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Screensaver, {
    .effectName = "Screensaver",
    .description = "You remember that DvD screensaver back in the day? This is that, but Freelancer.",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});