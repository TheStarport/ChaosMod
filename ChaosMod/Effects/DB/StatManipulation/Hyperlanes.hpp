#pragma once
#include "Effects/MemoryEffect.hpp"

class Hyperlanes final : public MemoryEffect
{
        // clang-format off
        MemoryListStart(offsets)
        MemoryListItem("common.dll", TradelaneSpeed, sizeof(float))
        MemoryListEnd(offsets);
        // clang-format on

        void Begin() override
        {
            MemoryEffect::Begin();

            float newSpeed = 20000.0f;
            MemUtils::WriteProcMem(offsets[0].module + offsets[0].offset, &newSpeed, sizeof(float));
        }

    public:
        DefEffectInfo("Hyperlanes", 2.0f, EffectType::StatManipulation);
};
