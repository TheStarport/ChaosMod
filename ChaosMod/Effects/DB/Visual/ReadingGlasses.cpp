#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class ReadingGlasses final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("Magnifier", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("Magnifier", false); }

    public:
        explicit ReadingGlasses(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(ReadingGlasses, {
    .effectName = "Reading Glasses",
    .description = "Sorry my eyesight is a little bad. Let me put on my reading glasses so I can see what I am focusing on a little better...",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::Visual
});