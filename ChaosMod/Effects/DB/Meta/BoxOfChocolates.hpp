#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/ImguiComponents/ActiveEffectsText.hpp"

class BoxOfChocolates final : public ActiveEffect
{
        void Begin() override { ActiveEffectsText::BoxOfChocolates(); }
        void End() override { ActiveEffectsText::BoxOfChocolates(); }

    public:
        DefaultEffectInfo("Box Of Chocolates", EffectType::Meta);
};
