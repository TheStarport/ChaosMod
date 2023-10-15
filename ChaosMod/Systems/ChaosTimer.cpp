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
#include "Effects/DB/Audio/SadViolin.hpp"
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
#include "Effects/DB/Misc/SelfDestruct.hpp"
#include "Effects/DB/Misc/SpectatorMode.hpp"
#include "Effects/DB/Misc/ThanksIHateIt.hpp"
#include "Effects/DB/Misc/XboxEdition.hpp"
#include "Effects/DB/Movement/AssumingDirectControl.hpp"
#include "Effects/DB/Movement/IllTrySpinning.hpp"
#include "Effects/DB/Movement/InvertVelocity.hpp"
#include "Effects/DB/Movement/PersonalSpace.hpp"
#include "Effects/DB/Movement/Yeet.hpp"
#include "Effects/DB/NPC/Cardamine.hpp"
#include "Effects/DB/NPC/IAmRobot.hpp"
#include "Effects/DB/NPC/MultiplayerExperience.hpp"
#include "Effects/DB/NPC/Pacifist.hpp"
#include "Effects/DB/NPC/PlanetOfTheApes.hpp"
#include "Effects/DB/NPC/Screaming.hpp"
#include "Effects/DB/NPC/SwiftNPCs.hpp"
#include "Effects/DB/NPC/SwissDiplomacy.hpp"
#include "Effects/DB/NPC/Wanted.hpp"
#include "Effects/DB/NPC/XenosAreMyFriendNow.hpp"
#include "Effects/DB/NPC/YouAreFamous.hpp"
#include "Effects/DB/Spawning/Hydra.hpp"
#include "Effects/DB/Spawning/JellyTime.hpp"
#include "Effects/DB/Spawning/SpawnBigBertha.hpp"
#include "Effects/DB/Spawning/SpawnExtremeJesus.hpp"
#include "Effects/DB/Spawning/SpawnJesus.hpp"
#include "Effects/DB/StatManipulation/CoolantLeak.hpp"
#include "Effects/DB/StatManipulation/DemolitionDerby.hpp"
#include "Effects/DB/StatManipulation/DoubleDamage.hpp"
#include "Effects/DB/StatManipulation/GetHumbled.hpp"
#include "Effects/DB/StatManipulation/GodMode.hpp"
#include "Effects/DB/StatManipulation/Hyperlanes.hpp"
#include "Effects/DB/StatManipulation/LonniganCameThrough.hpp"
#include "Effects/DB/StatManipulation/NeverSayNoToBacta.hpp"
#include "Effects/DB/StatManipulation/RandomisedProjectiles.hpp"
#include "Effects/DB/StatManipulation/TryRepairing.hpp"
#include "Effects/DB/StatManipulation/WiffleBats.hpp"
#include "Effects/DB/StatManipulation/Zoomies.hpp"
#include "Effects/DB/Teleport/AintThatOneABeauty.hpp"
#include "Effects/DB/Teleport/FakeTeleport.hpp"
#include "Effects/DB/Teleport/RandomSystem.hpp"
#include "Effects/DB/Teleport/TakeABreak.hpp"
#include "Effects/DB/Teleport/TheStarsReallyAreBeautiful.hpp"
#include "Effects/DB/Teleport/UrgenRelativeVisit.hpp"
#include "Effects/DB/Visual/AllWeKnowIsThatThereWereTwoSides.hpp"
#include "Effects/DB/Visual/HomageToElite.hpp"
#include "Effects/DB/Visual/PartyLikeIts95.hpp"
#include "Effects/DB/Visual/PortraitMode.hpp"
#include "Effects/DB/Visual/QuakeFOV.hpp"
#include "Effects/DB/Visual/RookieBirthdayParty.hpp"
#include "Effects/DB/Visual/Screensaver.hpp"
#include "Effects/DB/Visual/SleepyPlayer.hpp"
#include "UiManager.hpp"

#include <magic_enum.hpp>

void ChaosTimer::ShipDestroyed(DamageList* dmgList, DWORD* ecx, uint kill)
{
    auto* ship = reinterpret_cast<CShip*>(ecx[4]);
    for (const auto& key : i()->activeEffects | std::views::keys)
    {
        key->OnShipDestroyed(dmgList, ship);
    }

    for (const auto& effect : i()->persistentEffects)
    {
        if (effect->Persisting())
        {
            effect->OnShipDestroyed(dmgList, ship);
        }
    }
}

__declspec(naked) void ChaosTimer::NakedShipDestroyed()
{
    __asm {
        mov eax, [esp+0Ch] ; +4
        mov edx, [esp+4]
        push ecx
        push edx
        push ecx
        push eax
        call ShipDestroyed
        pop ecx
        mov eax, [oldShipDestroyed]
        jmp eax
    }
}

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

    for (uint attempts = 0; attempts < 15; attempts++)
    {
        const auto val = Random::i()->Uniform(0u, possibleEffects.size() - 1);
        auto effect = possibleEffects[val];

        if (const auto persistent = dynamic_cast<PersistentEffect*>(effect))
        {
            if (persistent->Persisting())
            {
                continue;
            }

            // If its not already persisting, lets start the effect!
            effect->Begin();
        }

        if (activeEffects.contains(effect))
        {
            continue;
        }

        if (auto& effectInfo = effect->GetEffectInfo();
            effectInfo.exclusion != EffectExclusion::None && std::ranges::any_of(activeEffects,
                                                                                 [&effectInfo](auto existingEffect)
                                                                                 {
                                                                                     auto existingInfo = existingEffect.first->GetEffectInfo();
                                                                                     return existingInfo.exclusion == effectInfo.exclusion;
                                                                                 }))
        {
            continue;
        }

        return effect;
    }

    Log("Failed to select effect after 15 attempts.");
    return nullptr;
}

void ChaosTimer::TriggerChaos(ActiveEffect* effect)
{
    currentTime = 0.0f;

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

ChaosTimer::ChaosTimer()
{
    auto shipDestroyedAddress = reinterpret_cast<PDWORD>(NakedShipDestroyed);

    const auto offset = RelOfs("server.dll", AddressTable::ShipDestroyedFunction);
    MemUtils::ReadProcMem(offset, &oldShipDestroyed, 4);
    MemUtils::WriteProcMem(offset, &shipDestroyedAddress, 4);
}

void ChaosTimer::DelayActiveEffect(ActiveEffect* effect, const float delay)
{
    if (const auto found = activeEffects.find(effect); found != activeEffects.end())
    {
        found->second += delay;
    }
}

void ChaosTimer::ToggleDoubleTime() { doubleTime = !doubleTime; }
void ChaosTimer::AdjustModifier(const float modifier) { modifiers += modifier; }

void ChaosTimer::FrameUpdate(const float delta)
{
    for (const auto& key : activeEffects | std::views::keys)
    {
        key->FrameUpdate(delta);
    }

    for (const auto& effect : persistentEffects)
    {
        if (effect->Persisting())
        {
            effect->FrameUpdate(delta);
        }
    }
}

void ChaosTimer::InitEffects()
{
    for (const auto possibleEffects = ActiveEffect::GetAllEffects(); const auto& effect : possibleEffects)
    {
        effect->Init();

        if (const auto persistent = dynamic_cast<PersistentEffect*>(effect))
        {
            persistentEffects.emplace_back(persistent);
        }
    }
}

float ChaosTimer::GetTimeUntilChaos() const { return ConfigManager::i()->timeBetweenChaos - currentTime; }

std::vector<ActiveEffect*> ChaosTimer::GetNextEffects(const int count)
{
    std::vector<ActiveEffect*> effects;
    for (int i = 0; i < count; i++)
    {
        const auto effect = SelectEffect();
        if (!effect || std::ranges::find(effects, effect) != effects.end())
        {
            i--;
            continue;
        }

        effects.emplace_back(effect);
    }

    return effects;
}

void ChaosTimer::OnApplyDamage(const uint hitSpaceObj, DamageList* dmgList, DamageEntry& dmgEntry, const bool after)
{
    for (const auto& key : i()->activeEffects | std::views::keys)
    {
        after ? key->OnApplyDamageAfter(hitSpaceObj, dmgList, dmgEntry) : key->OnApplyDamage(hitSpaceObj, dmgList, dmgEntry);
    }

    for (const auto& effect : i()->persistentEffects)
    {
        if (effect->Persisting())
        {
            after ? effect->OnApplyDamageAfter(hitSpaceObj, dmgList, dmgEntry) : effect->OnApplyDamage(hitSpaceObj, dmgList, dmgEntry);
        }
    }
}

void ChaosTimer::Update(const float delta)
{
    // If they don't have a ship lets not do chaos (aka are they in space?)
    const auto currentShip = Utils::GetCShip();
    if (!currentShip)
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
                // Don't clear persistent effects
                if (!dynamic_cast<PersistentEffect*>(key))
                {
                    key->End();
                }
            }

            activeEffects.clear();
            currentTime = 0.0f;
        }

        return;
    }

    if (currentShip != lastPlayerShip)
    {
        for (const auto effect : activeEffects | std::views::keys)
        {
            effect->OnLoad();
        }

        lastPlayerShip = currentShip;
    }

    currentTime += delta;

    if (const float nextTime = GetTimeUntilChaos(); !ConfigManager::i()->enableTwitchVoting && currentTime > nextTime)
    {
        TriggerChaos();

        if (doubleTime)
        {
            TriggerChaos();
        }
    }

    UiManager::i()->UpdateProgressBar(currentTime / ConfigManager::i()->timeBetweenChaos);

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
            // If its not a persistent effect call the end
            if (!dynamic_cast<PersistentEffect*>(effect->first))
            {
                effect->first->End();
            }
            effect = activeEffects.erase(effect);
        }
        else
        {
            ++effect;
        }
    }

    // For every persistent effect we should play its update too
    for (const auto persistent : persistentEffects)
    {
        if (persistent->Persisting())
        {
            persistent->Update(delta);
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
    Ef(SadViolin);

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
    Ef(NeverSayNoToBacta);
    Ef(DoubleDamage);

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
    Ef(ThanksIHateIt);

    // Movement
    Ef(Yeet);
    Ef(IllTrySpinning);
    Ef(InvertVelocity);
    Ef(PersonalSpace);
    Ef(AssumingDirectControl);

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
    Ef(SwiftNPCs);
    Ef(Pacifist);

    // Spawning
    Ef(Hydra);
    Ef(SpawnBigBertha);
    Ef(SpawnJesus);
    Ef(SpawnExtremeJesus);
    Ef(JellyTime);

    // Teleports
    Ef(TakeABreak);
    Ef(UrgentRelativeVisit);
    Ef(RandomSystem);
    Ef(AintThatOneABeauty);
    Ef(TheStarsReallyAreBeautiful);
    Ef(FakeTeleport);

    // Visual
    Ef(QuakeFov);
    Ef(Screensaver);
    Ef(HomageToElite);
    Ef(PartyLikeIts95);
    Ef(SleepyPlayer);
    Ef(PortraitMode);
    Ef(AllWeKnowIsThatThereWereTwoSides);
    Ef(RookieBirthdayParty);

#undef Ef
}
