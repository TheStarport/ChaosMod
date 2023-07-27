#pragma once
#include "Systems/CameraController.hpp"

class Hindsight final : public ActiveEffect
{
        void Update(float delta) override { CameraController::i()->ForceRearView(true); }

        void End() override { CameraController::i()->ForceRearView(false); }

    public:
        DefEffectInfo("Hindsight", 1.0f, EffectType::Visual);
};
