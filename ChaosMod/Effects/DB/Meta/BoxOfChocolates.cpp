#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ImguiComponents/ImGuiManager.hpp"

class BoxOfChocolates final : public ActiveEffect
{
        void Begin() override { ImGuiManager::ToggleBoxOfChocolates(true); }
        void End() override { ImGuiManager::ToggleBoxOfChocolates(false); }

    public:
        explicit BoxOfChocolates(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(BoxOfChocolates, {
    .effectName = "Box Of Chocolates",
    .description = "Hides what effects are currently active because life is like a box of chocolates... you never know what you're gonna get.",
    .category = EffectType::Meta,
    .timingModifier = 2.0f,
});