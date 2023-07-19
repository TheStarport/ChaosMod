#pragma once

#include "ActiveEffect.hpp"

class MemoryEffect : public ActiveEffect
{
        static std::vector<MemoryEffect*> memoryChanges;

    public:
        struct MemoryAddress
        {
                DWORD module;
                uint offset;
                size_t length;
                std::vector<byte> originalData;
        };

        static std::vector<MemoryAddress> GetMemoryEffects();

        void Begin() override;
        void End() override;

    protected:
        virtual std::vector<MemoryAddress>& GetMemoryAddresses() = 0;
};

// clang-format off
#define MemoryListStart(x) std::vector<MemoryAddress> x = {
#define MemoryListItem(module, offset, length) { DWORD(GetModuleHandleA(module)), offset, length },
#define MemoryListEnd(x) };                                                                                 \
    std::vector<MemoryAddress>& GetMemoryAddresses()                                                  \
    {                                                                                                       \
        return x;                                                                                           \
    }

// clang-format on
