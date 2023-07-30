#pragma once

class Zoomies final : public ActiveEffect
{
        void Begin() override { EngineEquipConsts::CRUISING_SPEED *= 2.0f; }

        void End() override { EngineEquipConsts::CRUISING_SPEED *= 0.5f; }

    public:
        DefEffectInfo("Zoomies", 1.0f, EffectType::StatManipulation);
};
