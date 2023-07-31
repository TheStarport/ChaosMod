#pragma once
#include "Systems/DirectX/Drawing.hpp"

class HomageToElite final : public ActiveEffect
{
        void FrameUpdate(float delta) override { DrawingHelper::i()->SetRenderState(D3DRS_FILLMODE, 2, true); }
        void End() override { DrawingHelper::i()->SetRenderState(D3DRS_FILLMODE, 3, true); }

    public:
        DefEffectInfo("Homage To Elite", 1.0f, EffectType::Visual);
};
