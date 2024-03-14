#include "PCH.hpp"

#include "Effects/PersistentEffect.hpp"

class WideBoi final : public PersistentEffect
{
        // TODO: make this effect not crap
        void Begin() override
        {
            PersistentEffect::Begin();
            float newTractorBeamWidthMultiplier = 200.0f;
            MemUtils::WriteProcMem(reinterpret_cast<DWORD>(GetModuleHandle(nullptr)) + 0x1C9800, &newTractorBeamWidthMultiplier, sizeof(float));
        }

    public:
        explicit WideBoi(const EffectInfo& effectInfo) : PersistentEffect(effectInfo) {}
};

// clang-format off
SetupEffect(WideBoi, {
    .effectName = "WIDE BOI",
    .description = "What if your tractor beam was THICC? Honestly this isn't a very good effect.",
    .category = EffectType::Visual,
    .timingModifier = 0.0f,
    .isTimed = false,
});