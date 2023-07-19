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
        };

        static std::vector<MemoryAddress> GetMemoryEffects();

        void Begin() override;
        void End() override;

    protected:
        std::vector<byte> originalData;
        virtual const MemoryAddress& GetMemoryAddress() = 0;
};

#define DefMemoryAddress(module, offset, length)                                        \
    const MemoryAddress& GetMemoryAddress()                                             \
    {                                                                                   \
        static MemoryAddress mem = { DWORD(GetModuleHandleA(module)), offset, length }; \
        return mem;                                                                     \
    }
