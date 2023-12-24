#include "PCH.hpp"

#include "MemoryEffect.hpp"
#include "magic_enum.hpp"

using namespace magic_enum::bitwise_operators;

std::vector<MemoryEffect*> MemoryEffect::memoryChanges;

std::vector<MemoryEffect::MemoryAddress> MemoryEffect::GetMemoryEffects()
{
    std::vector<MemoryAddress> ret;
    ret.reserve(memoryChanges.size());

    for (const auto& change : memoryChanges)
    {
        for (const auto& address : change->GetMemoryAddresses())
        {
            ret.emplace_back(address);
        }
    }

    return ret;
}

void MemoryEffect::Begin()
{
    for (auto& [module, offset, length, originalData] : GetMemoryAddresses())
    {
        originalData.resize(length);
        MemUtils::ReadProcMem(module + offset, originalData.data(), length);
    }

    memoryChanges.emplace_back(this);
}

void MemoryEffect::End()
{
    for (auto& [module, offset, length, originalData] : GetMemoryAddresses())
    {
        MemUtils::WriteProcMem(module + offset, originalData.data(), length);
    }

    std::erase(memoryChanges, this);
}
