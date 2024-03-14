#include "PCH.hpp"

#include "Effects/MemoryEffect.hpp"

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
        explicit DemolitionDerby(const EffectInfo& effectInfo) : MemoryEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DemolitionDerby, {
    .effectName = "Demolition Derby",
    .description = "Be careful when you collide with things. Colliding in space is dangerous!",
    .category = EffectType::StatManipulation
});
