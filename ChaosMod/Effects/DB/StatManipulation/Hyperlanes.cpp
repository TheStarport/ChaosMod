#include "PCH.hpp"

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
        explicit Hyperlanes(const EffectInfo& effectInfo) : MemoryEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Hyperlanes, {
    .effectName = "Hyperlanes",
    .description = "You've been given a SUPER DUPER SPEED UP POWER. Tradelanes are gonna make you go FAAAAAAST.",
    .category = EffectType::StatManipulation,
    .timingModifier = 3.0f
});
