#pragma once
#include "Systems/DirectX/Drawing.hpp"

class HomageToElite final : public ActiveEffect
{
        void FrameUpdate(float delta) override { DrawingHelper::i()->SetRenderState(D3DRS_FILLMODE, 2, true); }
        void End() override { DrawingHelper::i()->SetRenderState(D3DRS_FILLMODE, 3, true); }

    public:
        DefaultEffectInfo("Homage To Elite", EffectType::Visual);
};
