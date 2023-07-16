#include "PCH.hpp"

#include "ChaosTimer.hpp"

#include "UiManager.hpp"

void ChaosTimer::TriggerChaos()
{
    pub::Audio::PlaySoundEffect(1, CreateID("ui_close_infocard"));
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
