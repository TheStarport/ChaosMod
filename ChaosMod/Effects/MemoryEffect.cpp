#include "PCH.hpp"

#include "MemoryEffect.hpp"
#include "magic_enum.hpp"

using namespace magic_enum::bitwise_operators;

std::vector<std::unique_ptr<ActiveEffect>> ActiveEffect::allEffects;
std::vector<MemoryEffect*> MemoryEffect::memoryChanges;

std::vector<MemoryEffect::MemoryAddress> MemoryEffect::GetMemoryEffects()
{
    std::vector<MemoryAddress> ret;
    ret.reserve(memoryChanges.size());

    for (const auto& change : memoryChanges)
    {
        ret.emplace_back(change->GetMemoryAddress());
    }

    return ret;
}

void MemoryEffect::Begin()
{
    const auto [module, address, length] = GetMemoryAddress();
    originalData.resize(length);
    Utils::Memory::ReadProcMem(module + address, originalData.data(), length);
}

void MemoryEffect::End()
{
    const auto [module, address, length] = GetMemoryAddress();
    Utils::Memory::WriteProcMem(address, originalData.data(), length);
}
