#pragma once
#include "Systems/DirectX/Drawing.hpp"

class PortraitMode final : public ActiveEffect
{
        void FrameUpdate(float delta) override
        {
            const auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            const auto drawing = DrawingHelper::i();
            drawing->BoxFilled(0.f, 0.0f, 0.3333333f, 1.f, D3DCOLOR_XRGB(0, 0, 0));
            drawing->BoxFilled(0.6666666f, 0.0f, 0.3333333f, 1.f, D3DCOLOR_XRGB(0, 0, 0));
        }

    public:
        DefEffectInfo("Portrait Mode", 1.0f, EffectType::Visual);
};
