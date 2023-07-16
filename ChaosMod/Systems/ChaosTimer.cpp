#include "PCH.hpp"

#include "ChaosTimer.hpp"

#include "UiManager.hpp"

void ChaosTimer::TriggerChaos()
{
    // Do nothing for now
}

void ChaosTimer::Update(const float delta)
{
    currentTime += delta;

    const float nextTime = timeBetweenChaos * modifiers;
    if (currentTime > nextTime)
    {
        currentTime = 0.0f;
        TriggerChaos();
    }

    UiManager::i()->UpdateProgressBar(currentTime / nextTime);
}
