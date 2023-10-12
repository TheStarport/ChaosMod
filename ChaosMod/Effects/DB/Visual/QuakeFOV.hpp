#pragma once
#include "Systems/CameraController.hpp"

class QuakeFov final : public ActiveEffect
{
        void Begin() override { CameraController::i()->OverrideFov(120.0f); }
        void End() override { CameraController::i()->OverrideFov(0.0f); }

    public:
        DefEffectInfo("Quake FOV", .75f, EffectType::Visual);
};
