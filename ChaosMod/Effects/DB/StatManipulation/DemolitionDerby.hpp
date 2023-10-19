#pragma once

class DemolitionDerby final : public MemoryEffect
{
        // clang-format off
        MemoryListStart(offsets)
        MemoryListItem("common.dll", CollisionDamage, sizeof(float))
        MemoryListEnd(offsets);
        // clang-format on

        void Begin() override
        {
            MemoryEffect::Begin();
            const auto [module, offset, length, originalData] = offsets[0];
            float newCollisionDamageFactor = 40.0f; // original is .25f;
            MemUtils::WriteProcMem(module + offset, &newCollisionDamageFactor, sizeof(float));
        }

    public:
        DefaultEffectInfo("Demolition Derby", EffectType::StatManipulation);
};
