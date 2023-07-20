#pragma once

#include "Effects/ActiveEffect.hpp"

class ChaosTimer final : public Singleton<ChaosTimer>
{
        float currentTime = 0.0f;
        float modifiers = 1.0f;
        bool doubleTime = false;

        static void PlayBadEffect();
        static void PlayEffectSkip();
        static void PlayNextEffect();

        ActiveEffect* SelectEffect();
        void TriggerChaos(ActiveEffect* effect = nullptr);
        std::unordered_map<ActiveEffect*, float> activeEffects;

    public:
        void TriggerSpecificEffect(ActiveEffect* effect);
        void ToggleDoubleTime();
        void AdjustModifier(float modifier);
        void Update(float delta);
        static const std::unordered_map<ActiveEffect*, float>& GetActiveEffects();
        static void RegisterAllEffects();
};
