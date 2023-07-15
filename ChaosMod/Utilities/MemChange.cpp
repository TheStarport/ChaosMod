#include "PCH.hpp"

#include "MemChange.hpp"
#include "magic_enum.hpp"

using namespace magic_enum::bitwise_operators;

static std::unordered_map<MemEvent, std::vector<MemChange*>> memoryChanges;

MemChange::MemChange(const MemEvent desiredEvents, const DWORD address, const size_t length,
                     const std::function<void(MemEvent triggeringEvent, const std::vector<byte>& currentData, float frameDelta)> callback)
    : address(address), length(length), callback(callback)
{
    // If the memory change list hasn't been setup, iterate over the the possible events and create an empty vector
    if (memoryChanges.empty())
    {
        magic_enum::enum_for_each<MemEvent>(
            [](auto val)
            {
                if (constexpr MemEvent evt = val; evt != MemEvent::None)
                {
                    memoryChanges[evt] = std::vector<MemChange*>();
                }
            });
    }

    // Copy what the original value was
    originalData.resize(length);
    Utils::Memory::ReadProcMem(reinterpret_cast<void*>(address), originalData.data(), length);

    if ((desiredEvents & MemEvent::OnUpdate) != MemEvent::None)
    {
        memoryChanges[MemEvent::OnUpdate].emplace_back(this);
    }

    if ((desiredEvents & MemEvent::OnFixedUpdate) != MemEvent::None)
    {
        memoryChanges[MemEvent::OnFixedUpdate].emplace_back(this);
    }
}

void MemChange::TriggerEvent(const MemEvent event, const float frameDelta)
{
    for (const auto& memChange : memoryChanges[event])
    {
        std::vector<byte> currentData;
        currentData.resize(memChange->length);
        Utils::Memory::ReadProcMem(reinterpret_cast<void*>(memChange->address), currentData.data(), memChange->length);
        memChange->callback(event, currentData, frameDelta);
    }
}

void MemChange::Reset() { Utils::Memory::WriteProcMem(reinterpret_cast<void*>(address), originalData.data(), length); }

MemChange::~MemChange() { Reset(); }
