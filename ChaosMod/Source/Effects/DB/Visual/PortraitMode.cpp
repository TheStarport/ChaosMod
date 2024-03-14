#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "DirectX/Drawing.hpp"

class PortraitMode final : public ActiveEffect
{
        void FrameUpdate(float delta) override
        {
            const auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            const auto drawing = Get<DrawingHelper>();
            drawing->BoxFilled(0.f, 0.0f, 0.3333333f, 1.f, D3DCOLOR_XRGB(0, 0, 0));
            drawing->BoxFilled(0.6666666f, 0.0f, 0.3333333f, 1.f, D3DCOLOR_XRGB(0, 0, 0));
        }

    public:
        explicit PortraitMode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PortraitMode, {
    .effectName = "Portrait Mode",
    .description = "You really shouldn't have let your mum record the gameplay...",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});