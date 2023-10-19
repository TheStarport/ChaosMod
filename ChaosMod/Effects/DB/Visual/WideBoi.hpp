#pragma once
#include "Effects/PersistentEffect.hpp"

class WideBoi final : public PersistentEffect
{
        void Begin() override
        {
            float newTractorBeamWidthMultiplier = 200.0f;
            MemUtils::WriteProcMem(reinterpret_cast<DWORD>(GetModuleHandle(nullptr)) + 0x1C9800, &newTractorBeamWidthMultiplier, sizeof(float));
        }

    public:
        OneShotEffectInfo("WIDE BOI", EffectType::Visual);
};
