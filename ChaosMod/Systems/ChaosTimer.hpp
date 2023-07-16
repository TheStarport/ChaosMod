#pragma once

class ChaosTimer final : public Singleton<ChaosTimer>
{
        float currentTime = 0.0f;
        float timeBetweenChaos = 30.0f;
        float modifiers = 1.0f;

        void TriggerChaos();

    public:
        void Update(float delta);
};
