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

    public:
        void Begin() override
        {
            MemoryEffect::Begin();
            double height = 0.1;
            auto info = offsets[0];
            Utils::Memory::WriteProcMem(info.module + info.offset, &height, info.length);

            info = offsets[1];
            double width = 1.0;
            Utils::Memory::WriteProcMem(info.module + info.offset, &width, info.length);
        }

        DefEffectInfo("Buggy Interface", 1.0f, EffectType::Interface);
};
