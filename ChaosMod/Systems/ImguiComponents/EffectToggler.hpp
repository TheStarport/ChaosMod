#pragma once

#include "Effects/ActiveEffect.hpp"

class EffectToggler final
{
        bool show = false;
        std::map<EffectType, std::vector<ActiveEffect*>>* effects;

        void RenderEffectCategory();

    public:
        EffectToggler(std::map<EffectType, std::vector<ActiveEffect*>>* effects) : effects(effects) {}
        void Show();
        void Render();
};
