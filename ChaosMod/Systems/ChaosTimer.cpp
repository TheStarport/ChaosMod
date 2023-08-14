#include "PCH.hpp"

#include "ChaosTimer.hpp"

#include "ConfigManager.hpp"
#include "Effects/ActiveEffect.hpp"
#include "Effects/DB/Audio/AllTheNewsThatsFitForYou.hpp"
#include "Effects/DB/Audio/BossMusic.hpp"
#include "Effects/DB/Audio/DodgeThis.hpp"
#include "Effects/DB/Audio/IDoNotSpeakMorseCode.hpp"
#include "Effects/DB/Audio/IfTrentHadATextToSpeechDevice.hpp"
#include "Effects/DB/Audio/LowBudgetSciFiMovie.hpp"
#include "Effects/DB/Interface/BuggyInterface.hpp"
#include "Effects/DB/Interface/FlippedInterface.hpp"
#include "Effects/DB/Interface/JustLikeThe90s.hpp"
#include "Effects/DB/Interface/LGBTUI.hpp"
#include "Effects/DB/Interface/NoHud.hpp"
#include "Effects/DB/Interface/RollCredits.hpp"
#include "Effects/DB/Interface/StickyKeys.hpp"
#include "Effects/DB/Meta/BoxOfChocolates.hpp"
#include "Effects/DB/Meta/DoubleEffectTime.hpp"
#include "Effects/DB/Meta/DoubleTime.hpp"
#include "Effects/DB/Meta/FakeCrash.hpp"
#include "Effects/DB/Misc/CaveatEmptor.hpp"
#include "Effects/DB/Misc/FlappyArchitecture.hpp"
#include "Effects/DB/Misc/IAmFirinMaLasers.hpp"
#include "Effects/DB/Misc/LaggyPlayer.hpp"
#include "Effects/DB/Misc/RandomisedKeys.hpp"
#include "Effects/DB/Misc/SelfDestruct.hpp"
#include "Effects/DB/Misc/SpectatorMode.hpp"
#include "Effects/DB/Misc/XboxEdition.hpp"
#include "Effects/DB/Movement/IllTrySpinning.hpp"
#include "Effects/DB/Movement/InvertVelocity.hpp"
#include "Effects/DB/Movement/Yeet.hpp"
#include "Effects/DB/Movement/YouSpinMeRightRound.hpp"
#include "Effects/DB/NPC/Cardamine.hpp"
#include "Effects/DB/NPC/IAmRobot.hpp"
#include "Effects/DB/NPC/MultiplayerExperience.hpp"
#include "Effects/DB/NPC/PlanetOfTheApes.hpp"
#include "Effects/DB/NPC/Screaming.hpp"
#include "Effects/DB/NPC/SwissDiplomacy.hpp"
#include "Effects/DB/NPC/Wanted.hpp"
#include "Effects/DB/NPC/XenosAreMyFriendNow.hpp"
#include "Effects/DB/NPC/YouAreFamous.hpp"
#include "Effects/DB/StatManipulation/CoolantLeak.hpp"
#include "Effects/DB/StatManipulation/DemolitionDerby.hpp"
#include "Effects/DB/StatManipulation/GetHumbled.hpp"
#include "Effects/DB/StatManipulation/GodMode.hpp"
#include "Effects/DB/StatManipulation/Hyperlanes.hpp"
#include "Effects/DB/StatManipulation/LonniganCameThrough.hpp"
#include "Effects/DB/StatManipulation/RandomisedProjectiles.hpp"
#include "Effects/DB/StatManipulation/TryRepairing.hpp"
#include "Effects/DB/StatManipulation/WiffleBats.hpp"
#include "Effects/DB/StatManipulation/Zoomies.hpp"
#include "Effects/DB/Visual/AllWeKnowIsThatThereWereTwoSides.hpp"
#include "Effects/DB/Visual/HomageToElite.hpp"
#include "Effects/DB/Visual/PartyLikeIts95.hpp"
#include "Effects/DB/Visual/PortraitMode.hpp"
#include "Effects/DB/Visual/QuakeFOV.hpp"
#include "Effects/DB/Visual/Screensaver.hpp"
#include "Effects/DB/Visual/SleepyPlayer.hpp"
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
    float timing = info.isTimed ? modifiers * (info.timingModifier * ConfigManager::i()->defaultEffectDuration) : 30.0f;

    if (info.fixedTimeOverride != 0.0f)
    {
        timing = info.fixedTimeOverride;
    }

    activeEffects[effect] = timing;
    PlayNextEffect();
}

void ChaosTimer::TriggerSpecificEffect(ActiveEffect* effect) { TriggerChaos(effect); }

void ChaosTimer::ToggleDoubleTime() { doubleTime = !doubleTime; }
void ChaosTimer::AdjustModifier(const float modifier) { modifiers += modifier; }

void ChaosTimer::FrameUpdate(const float delta)
{
    for (const auto& key : activeEffects | std::views::keys)
    {
        key->FrameUpdate(delta);
    }
}

void ChaosTimer::InitEffects()
{
    const auto possibleEffects = ActiveEffect::GetAllEffects();
    for (const auto& effect : possibleEffects)
    {
        effect->Init();
    }
}

void ChaosTimer::Update(const float delta)
{
    // If they don't have a ship lets not do chaos (aka are they in space?)

    if (!Utils::GetCShip())
    {
        uint base;
        pub::Player::GetBase(1, base);
        // If they are not in a base, and don't have a ship we assume they are in the death screen
        if (!base)
        {
            return;
        }

        // Clear any ongoing effects if they are docked on a base
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
    Ef(FlippedUi);
    Ef(StickyKeys);
    Ef(RollCredits);
    Ef(JustLikeThe90s);

    // Audio
    Ef(AllTheNewsThatsFitForYou);
    Ef(BossMusic);
    Ef(IfTrentHadATextToSpeechDevice);
    Ef(IDoNotSpeakMorseCode);
    Ef(DodgeThis);
    Ef(LowBudgetSciFiMovie);

    // Stat Manipulation
    Ef(LonniganCameThrough);
    Ef(GetHumbled);
    Ef(WiffleBats);
    Ef(RandomisedProjectiles);
    Ef(GodMode);
    Ef(Hyperlanes);
    Ef(TryRepairing);
    Ef(CaveatEmptor);
    Ef(CoolantLeak);
    Ef(DemolitionDerby);
    Ef(Zoomies);

    // Meta
    Ef(BoxOfChocolates);
    Ef(FakeCrash);
    Ef(DoubleTime);
    Ef(DoubleEffectTime);

    // Misc
    Ef(FlappyArchitecture);
    Ef(SpectatorMode);
    Ef(LaggyPlayer);
    Ef(SelfDestruct);
    Ef(IAmFiringMyLasers);
    Ef(XboxEdition);
    Ef(RandomisedKeys);

    // Movement
    Ef(Yeet);
    Ef(YouSpinMeRightRound);
    Ef(IllTrySpinning);
    Ef(InvertVelocity);

    // Npc
    Ef(Screaming);
    Ef(IAmRobot);
    Ef(PlanetOfTheApes);
    Ef(MultiplayerExperience);
    Ef(Wanted);
    Ef(Cardamine);
    Ef(YouAreFamous);
    Ef(XenosAreMyFriendsNow);
    Ef(SwissDiplomacy);

    // Visual
    Ef(QuakeFov);
    Ef(Screensaver);
    Ef(HomageToElite);
    Ef(PartyLikeIts95);
    Ef(SleepyPlayer);
    Ef(PortraitMode);
    Ef(AllWeKnowIsThatThereWereTwoSides);

#undef Ef
}
