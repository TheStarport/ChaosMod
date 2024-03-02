#include "PCH.hpp"

#include "ChaosTimer.hpp"

#include "ConfigManager.hpp"
#include "Effects/ActiveEffect.hpp"

#include "Effects/AddressTable.hpp"
#include "ImguiComponents/ImGuiManager.hpp"
#include "PatchNotes/PatchNotes.hpp"
#include "UiManager.hpp"
#include "Utilities/OffsetHelper.hpp"

#include <magic_enum.hpp>

void ChaosTimer::ShipDestroyed(DamageList* dmgList, DWORD* ecx, uint kill)
{
    // Skip entries that are despawned, not destroyed (invalid memory)
    if (static_cast<int>(dmgList->isDestroyed) > 1 || dmgList->inflictorId > 0xffff)
    {
        return;
    }

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

    const auto& configEffects = ConfigManager::i()->toggledEffects;

    for (uint attempts = 0; attempts < 15; attempts++)
    {
        static std::vector<uint> weights;
        if (weights.empty())
        {
            std::ranges::for_each(possibleEffects,
                                  [](auto* effect)
                                  {
                                      auto& info = effect->GetEffectInfo();
                                      weights.emplace_back(info.weight);
                                  });
        }

        const auto val = Random::i()->Weighted(weights.begin(), weights.end());
        auto effect = possibleEffects[val];

        // If the precondition fails, or trying to teleport in a mission, prevent it.
        if (!effect->CanSelect())
        {
            continue;
        }

        if (ConfigManager::i()->blockTeleportsDuringMissions && OffsetHelper::IsInMission() && effect->GetEffectInfo().category == EffectType::Teleport)
        {
            continue;
        }

        if (const auto persistent = dynamic_cast<PersistentEffect*>(effect))
        {
            if (persistent->Persisting())
            {
                continue;
            }

            // If its not already persisting, lets start the effect!
            effect->Begin();
            break;
        }

        if (activeEffects.contains(effect))
        {
            continue;
        }

        auto& effectInfo = effect->GetEffectInfo();
        bool effectDisabled = false;
        for (const auto& category : configEffects)
        {
            if (std::ranges::any_of(category.second,
                                    [effectInfo](const std::pair<std::string, bool>& eff) { return !eff.second && eff.first == effectInfo.effectName; }))
            {
                effectDisabled = true;
                break;
            }
        }

        if (effectDisabled)
        {
            continue;
        }

        if (effectInfo.exclusion != EffectExclusion::None && std::ranges::any_of(activeEffects,
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
    patchNotes = new PatchNotes();
    patchTime = ConfigManager::i()->timeBetweenPatchesInMinutes * 60;

    auto shipDestroyedAddress = reinterpret_cast<PDWORD>(NakedShipDestroyed);

    const auto offset = RelOfs("server.dll", AddressTable::ShipDestroyedFunction);
    MemUtils::ReadProcMem(offset, &oldShipDestroyed, 4);
    MemUtils::WriteProcMem(offset, &shipDestroyedAddress, 4);
}

ChaosTimer::~ChaosTimer() { delete patchNotes; }

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
    // Assets loaded, we can now apply our patches
    PatchNotes::ResetPatches(true, false);

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

uint ChaosTimer::OnSoundEffect(const uint hash)
{
    for (const auto& key : i()->activeEffects | std::views::keys)
    {
        if (const uint newHash = key->OnSoundEffect(hash); newHash != hash)
        {
            return newHash;
        }
    }

    for (const auto& effect : i()->persistentEffects)
    {
        if (const uint newHash = effect->OnSoundEffect(hash); newHash != hash)
        {
            return newHash;
        }
    }

    return hash;
}

void ChaosTimer::Update(const float delta)
{
    // If they don't have a ship lets not do chaos (aka are they in space?)
    const auto currentShip = Utils::GetCShip();

    // Check that the game is not paused and patch notes are enabled
    if (ConfigManager::i()->enablePatchNotes && !OffsetHelper::IsGamePaused() && (ConfigManager::i()->countDownWhileOnBases || currentShip))
    {
        patchTime -= delta;
        if (patchTime <= 0.0f)
        {
            patchTime = ConfigManager::i()->timeBetweenPatchesInMinutes * 60;
            PatchNotes::GeneratePatch();
        }
    }

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

    if (!ConfigManager::i()->enableTwitchVoting && currentTime > ConfigManager::i()->timeBetweenChaos)
    {
        TriggerChaos();

        if (doubleTime)
        {
            TriggerChaos();
        }
    }

    ImGuiManager::SetProgressBarPercentage(currentTime / ConfigManager::i()->timeBetweenChaos);

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
