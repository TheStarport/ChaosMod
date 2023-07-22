#include "PCH.hpp"

#include "ChaosTimer.hpp"

#include "ConfigManager.hpp"
#include "Effects/ActiveEffect.hpp"
#include "Effects/DB/Audio/AllTheNewsThatsFitForYou.hpp"
#include "Effects/DB/Audio/BossMusic.hpp"
#include "Effects/DB/Audio/DodgeThis.hpp"
#include "Effects/DB/Audio/IDoNotSpeakMorseCode.hpp"
#include "Effects/DB/Audio/IfTrentHadATextToSpeechDevice.hpp"
#include "Effects/DB/Interface/BuggyInterface.hpp"
#include "Effects/DB/Interface/LGBTUI.hpp"
#include "Effects/DB/Interface/NoHud.hpp"
#include "Effects/DB/Meta/BoxOfChocolates.hpp"
#include "Effects/DB/Meta/DoubleEffectTime.hpp"
#include "Effects/DB/Meta/DoubleTime.hpp"
#include "Effects/DB/Meta/FakeCrash.hpp"
#include "Effects/DB/Misc/FlappyArchitecture.hpp"
#include "Effects/DB/NPC/IAmRobot.hpp"
#include "Effects/DB/NPC/PlanetOfTheApes.hpp"
#include "Effects/DB/NPC/Screaming.hpp"
#include "Effects/DB/StatManipulation/GetHumbled.hpp"
#include "Effects/DB/StatManipulation/LonniganCameThrough.hpp"
#include "Effects/DB/StatManipulation/SpectatorMode.hpp"
#include "Effects/DB/StatManipulation/WiffleBats.hpp"
#include "UiManager.hpp"

#include <magic_enum.hpp>

void ChaosTimer::PlayBadEffect() { pub::Audio::PlaySoundEffect(1, CreateID("ui_select_remove")); }
void ChaosTimer::PlayEffectSkip() { pub::Audio::PlaySoundEffect(1, CreateID("ui_open_infocard_button")); }
void ChaosTimer::PlayNextEffect() { pub::Audio::PlaySoundEffect(1, CreateID("ui_close_infocard")); }

ActiveEffect* ChaosTimer::SelectEffect()
{
    const auto possibleEffects = ActiveEffect::GetAllEffects();

    if (possibleEffects.empty())
    {
        Log("POSSIBLE EFFECTS LIST EMPTY");
        return nullptr;
    }

    for (uint attempts = 0; attempts < 5; attempts++)
    {
        const auto val = Random::i()->Uniform(0u, possibleEffects.size() - 1);
        auto effect = possibleEffects[val];
        if (activeEffects.contains(effect))
        {
            continue;
        }

        return effect;
    }

    Log("Failed to select effect after 5 attempts.");
    return nullptr;
}

void ChaosTimer::TriggerChaos(ActiveEffect* effect)
{
    if (!effect)
    {
        effect = SelectEffect();
        if (!effect)
        {
            PlayBadEffect();
            return;
        }
    }

    // Too many active effects
    if (activeEffects.size() >= ConfigManager::i()->totalAllowedConcurrentEffects)
    {
        // Clean out those without timers
        std::erase_if(activeEffects, [](const auto& active) { return !active.first->GetEffectInfo().isTimed; });
        if (activeEffects.size() >= ConfigManager::i()->totalAllowedConcurrentEffects)
        {
            // If still too large skip
            PlayEffectSkip();
            return;
        }
    }

    auto& info = effect->GetEffectInfo();
    Log(std::format("Starting Effect: {} ({})", info.effectName, magic_enum::enum_name(info.category)));
    effect->Begin();

    // Set the timing for this effect. If it's not a timed effect, default to 30s to clear it from the list.
    const float timing = info.isTimed ? modifiers * (info.timingModifier * ConfigManager::i()->defaultEffectDuration) : 30.0f;

    activeEffects[effect] = timing;
    PlayNextEffect();
}

void ChaosTimer::TriggerSpecificEffect(ActiveEffect* effect) { TriggerChaos(effect); }

void ChaosTimer::ToggleDoubleTime() { doubleTime = !doubleTime; }
void ChaosTimer::AdjustModifier(const float modifier) { modifiers += modifier; }

void ChaosTimer::Update(const float delta)
{
    // If they don't have a ship lets not do chaos (aka are they in space?)
    if (!Utils::GetCShip())
    {
        // Clear any ongoing effects
        if (!activeEffects.empty())
        {
            for (const auto& key : activeEffects | std::views::keys)
            {
                key->End();
            }

            activeEffects.clear();
            currentTime = 0.0f;
        }

        return;
    }

    currentTime += delta;

    const float nextTime = ConfigManager::i()->timeBetweenChaos * modifiers;
    if (currentTime > nextTime)
    {
        currentTime = 0.0f;
        TriggerChaos();

        if (doubleTime)
        {
            TriggerChaos();
        }
    }

    UiManager::i()->UpdateProgressBar(currentTime / nextTime);

    // Trigger chaos updates
    auto effect = std::begin(activeEffects);

    while (effect != std::end(activeEffects))
    {
        auto& info = effect->first->GetEffectInfo();
        if (info.isTimed)
        {
            effect->first->Update(delta);
        }

        effect->second -= delta;

        if (effect->second <= 0)
        {
            effect->first->End();
            effect = activeEffects.erase(effect);
        }
        else
        {
            ++effect;
        }
    }
}

const std::unordered_map<ActiveEffect*, float>& ChaosTimer::GetActiveEffects() { return i()->activeEffects; }

void ChaosTimer::RegisterAllEffects()
{
#define Ef(T) ActiveEffect::RegisterEffect<T>()

    // Interface
    Ef(Lgbtui);
    Ef(BuggyInterface);
    Ef(NoHud);

    // Audio
    Ef(AllTheNewsThatsFitForYou);
    Ef(BossMusic);
    Ef(IfTrentHadATextToSpeechDevice);
    Ef(IDoNotSpeakMorseCode);
    Ef(DodgeThis);

    // Stat Manipulation
    Ef(LonniganCameThrough);
    Ef(GetHumbled);
    Ef(WiffleBats);
    Ef(SpectatorMode);

    // Meta
    Ef(BoxOfChocolates);
    Ef(FakeCrash);
    Ef(DoubleTime);
    Ef(DoubleEffectTime);

    // Misc
    Ef(FlappyArchitecture);

    // Npc
    Ef(Screaming);
    Ef(IAmRobot);
    Ef(PlanetOfTheApes);

#undef Ef
}
