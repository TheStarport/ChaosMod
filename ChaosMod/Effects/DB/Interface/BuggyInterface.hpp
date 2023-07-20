// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

#include "Effects/MemoryEffect.hpp"

class BuggyInterface final : public MemoryEffect
{
        // clang-format off
        MemoryListStart(offsets)
        MemoryListItem("freelancer.exe", EnergyBarHeight, sizeof(double))
        MemoryListItem("freelancer.exe", TotalBarWidth, sizeof(double))
        MemoryListEnd(offsets);
        // clang-format on

        double width = 1.0;
        double height = 0.1;
        float time = 1.0f;
        bool goingDown = true;

    public:
        void Begin() override
        {
            width = 1.0;
            height = 0.1;
            goingDown = true;

            MemoryEffect::Begin();
            auto info = offsets[0];
            Utils::Memory::WriteProcMem(info.module + info.offset, &height, info.length);

            info = offsets[1];
            Utils::Memory::WriteProcMem(info.module + info.offset, &width, info.length);
        }

        void Update(const float delta) override
        {
            if (goingDown)
            {
                width = std::lerp(width, 0.052, delta * 2);
                height = std::lerp(height, 0.0052, delta * 2);
                if (height <= 0.0053)
                {
                    goingDown = false;
                }
            }
            else
            {
                width = std::lerp(width, 1, delta * 2);
                height = std::lerp(height, 0.1, delta * 2);

                if (height >= 0.099)
                {
                    goingDown = true;
                }
            }

            auto info = offsets[0];
            Utils::Memory::WriteProcMem(info.module + info.offset, &height, info.length);

            info = offsets[1];
            Utils::Memory::WriteProcMem(info.module + info.offset, &width, info.length);
        }

        DefEffectInfo("Buggy Interface", 1.0f, EffectType::Interface);
};
