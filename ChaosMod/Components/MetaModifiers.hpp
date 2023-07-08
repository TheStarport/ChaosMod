#pragma once

#include "Component.hpp"

#include <cstdint>

class MetaModifiers : public Component
{
    public:
        float effectDurationModifier = 1.f;
        float timerSpeedModifier = 1.f;
        std::uint8_t additionalEffectsToDispatch = 0;
        bool hideChaosUi = false;
        bool disableChaos = false;
        bool flipChaosUi = false;

        template <class T>
            requires std::is_base_of_v<Component, T>
        friend struct ComponentHolder;
};
