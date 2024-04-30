#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Effects/MemoryEffect.hpp"

class Tachycardia final : public MemoryEffect
{
    using MemoryEffect::MemoryEffect;

    // clang-format off
    MemoryListStart(offsets)
    MemoryListItem("content.dll", NpcDensityCapRange, sizeof(float))
    MemoryListEnd(offsets);
    // clang-format on

    void Begin() override
    {
        MemoryEffect::Begin();

        // Disable density cap
        constexpr float newRange = 0.0f;
        MemUtils::WriteProcMem(offsets[0].module + offsets[0].offset, &newRange, sizeof(float));

        const auto& spacePopController = static_cast<const uint&>(reinterpret_cast<uint>(GetModuleHandleA("content.dll")) + 0x12FE64);
        pub::Controller::SetHeartbeatInterval(spacePopController, 0.5f);
    }

    void End() override
    {
        MemoryEffect::End();

        const auto& spacePopController = static_cast<const uint&>(reinterpret_cast<uint>(GetModuleHandleA("content.dll")) + 0x12FE64);
        pub::Controller::SetHeartbeatInterval(spacePopController, 3.f);
    }
};

// clang-format off
SetupEffect(Tachycardia, {
    .effectName = "Tachycardia",
    .description = "Perhaps the NPCs should go to the hospital. They have a very fast heart rate.",
    .category = EffectType::Npc
});