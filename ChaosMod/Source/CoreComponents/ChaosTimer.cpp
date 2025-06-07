#include "PCH.hpp"

#include "CoreComponents/ChaosTimer.hpp"

#include "Components/ConfigManager.hpp"
#include "Components/DiscordManager.hpp"
#include "Components/ShipManipulator.hpp"
#include "Effects/ActiveEffect.hpp"

#include "Components/GlobalTimers.hpp"
#include "Components/UiManager.hpp"
#include "CoreComponents/PatchNotes.hpp"
#include "Effects/AddressTable.hpp"
#include "ImGui/ImGuiManager.hpp"
#include "Memory/OffsetHelper.hpp"

#include <magic_enum.hpp>

void ChaosTimer::ShipDestroyed(DamageList* dmgList, DWORD* ecx, uint kill)
{
    // Skip entries that are despawned, not destroyed (invalid memory)
    if (static_cast<int>(dmgList->isDestroyed) > 1 || dmgList->inflictorId > 0xffff)
    {
        return;
    }

    auto* ship = reinterpret_cast<CShip*>(ecx[4]);
    const auto i = Get<ChaosTimer>();
    for (const auto& key : i->activeEffects | std::views::keys)
    {
        key->OnShipDestroyed(dmgList, ship);
    }

    for (const auto& effect : i->persistentEffects)
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

void ChaosTimer::OnMunitionHit(EqObj* hitObject, MunitionImpactData* impact, DamageList* dmgList, const bool after)
{
    const auto i = Get<ChaosTimer>();
    for (const auto& key : i->activeEffects | std::views::keys)
    {
        after ? key->OnMunitionHitAfter(hitObject, impact, dmgList) : key->OnMunitionHit(hitObject, impact, dmgList);
    }

    for (const auto& effect : i->persistentEffects)
    {
        if (effect->Persisting())
        {
            after ? effect->OnMunitionHitAfter(hitObject, impact, dmgList) : effect->OnMunitionHit(hitObject, impact, dmgList);
        }
    }
}

bool ChaosTimer::OnExplosion(EqObj* hitObject, ExplosionDamageEvent* explosion, DamageList* dmgList)
{
    const auto i = Get<ChaosTimer>();
    for (const auto& key : i->activeEffects | std::views::keys)
    {
        if (!key->OnExplosion(hitObject, explosion, dmgList))
        {
            return true;
        }
    }

    for (const auto& effect : i->persistentEffects)
    {
        if (effect->Persisting() && !effect->OnExplosion(hitObject, explosion, dmgList))
        {
            return true;
        }
    }

    return false;
}

uint ChaosTimer::OnSoundEffect(const uint hash)
{
    const auto i = Get<ChaosTimer>();

    for (const auto& key : i->activeEffects | std::views::keys)
    {
        if (const uint newHash = key->OnSoundEffect(hash); newHash != hash)
        {
            return newHash;
        }
    }

    for (const auto& effect : i->persistentEffects)
    {
        if (const uint newHash = effect->OnSoundEffect(hash); newHash != hash)
        {
            return newHash;
        }
    }

    return hash;
}

void ChaosTimer::OnSystemUnload()
{
    const auto i = Get<ChaosTimer>();

    for (const auto& effect : ActiveEffect::GetAllEffects())
    {
        effect->OnSystemUnload();
    }
}

void ChaosTimer::OnJumpInComplete()
{
    const auto i = Get<ChaosTimer>();

    for (const auto& key : i->activeEffects | std::views::keys)
    {
        key->OnJumpInComplete();
    }

    for (const auto& effect : i->persistentEffects)
    {
        effect->OnJumpInComplete();
    }
}

FireResult __fastcall ChaosTimer::OnCanFire(CEGun* gun, void* edx, const Vector& target)
{
    static auto moneyGun = CreateID("chaos_plan_b");
    if (gun->archetype->archId == moneyGun)
    {
        int money;
        pub::Player::InspectCash(1, money);
        if (constexpr int cost = 4; money < cost)
        {
            return FireResult::AmmoRequirementsNotMet;
        }
    }

    canFireDetour.UnDetour();
    const auto result = canFireDetour.GetOriginalFunc()(gun, edx, target);
    canFireDetour.Detour(OnCanFire);
    return result;
}

void __fastcall ChaosTimer::OnConsumeFireResources(CELauncher* launcher)
{
    const auto i = Get<ChaosTimer>();

    for (const auto& key : i->activeEffects | std::views::keys)
    {
        key->OnConsumeFireResources(launcher);
    }

    for (const auto& effect : i->persistentEffects)
    {
        effect->OnConsumeFireResources(launcher);
    }

    static auto moneyGun = CreateID("chaos_plan_b");
    if (launcher->archetype->archId == moneyGun)
    {
        int money;
        pub::Player::InspectCash(1, money);
        constexpr int cost = -4;
        if (money < cost)
        {
            return;
        }

        pub::Player::AdjustCash(1, cost);
    }

    consumeFireResourcesDetour.UnDetour();
    consumeFireResourcesDetour.GetOriginalFunc()(launcher);
    consumeFireResourcesDetour.Detour(OnConsumeFireResources);

    static uint blunderBuss = CreateID("chaos_blunderbuss");

    if (launcher->archetype->archId == blunderBuss)
    {
        auto ship = launcher->GetOwner();

        auto orientation = ship->get_orientation();
        Vector forwards = { orientation[0][2], orientation[1][2], orientation[2][2] };
        forwards *= 1000.f;

        Vector newVelocity = ShipManipulator::GetVelocity(ship) + forwards;
        Get<GlobalTimers>()->AddTimer(
            [ship, newVelocity](float delta)
            {
                ShipManipulator::SetVelocity(ship, newVelocity);
                return true;
            },
            0.05f);
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

    const auto& configEffects = Get<ConfigManager>()->chaosSettings.toggledEffects;

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

        const auto val = Get<Random>()->Weighted(weights.begin(), weights.end());
        auto effect = possibleEffects[val];

        // If the precondition fails, or trying to teleport in a mission, prevent it.
        if (!effect->CanSelect())
        {
            continue;
        }

        if (Get<ConfigManager>()->chaosSettings.blockTeleportsDuringMissions && OffsetHelper::IsInMission() &&
            effect->GetEffectInfo().category == EffectType::Teleport)
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
    if (activeEffects.size() >= Get<ConfigManager>()->chaosSettings.totalAllowedConcurrentEffects)
    {
        // Clean out those without timers
        std::erase_if(activeEffects, [](const auto& active) { return !active.first->GetEffectInfo().isTimed; });
        if (activeEffects.size() >= Get<ConfigManager>()->chaosSettings.totalAllowedConcurrentEffects)
        {
            // If still too large skip
            PlayEffectSkip();
            return;
        }
    }

    const auto& info = effect->GetEffectInfo();
    Log(std::format("Starting Effect: {} ({})", info.effectName, magic_enum::enum_name(info.category)));

    ImGuiManager::AddToEffectHistory(info.effectName, info.description);

    // Set the timing for this effect. If it's not a timed effect, default to 30s to clear it from the list.
    float timing = info.isTimed ? modifiers * (info.timingModifier * Get<ConfigManager>()->chaosSettings.defaultEffectDuration) : 30.0f;

    if (info.fixedTimeOverride != 0.0f)
    {
        timing = info.fixedTimeOverride;
    }

    // Set the time then call the begin function (in case an effect wants to increase its own duration)
    activeEffects[effect] = timing;
    effect->Begin();

    // Play next effect audio
    PlayNextEffect();

    // Force discord update
    timeSinceLastUpdate = 0.f;
}

ChaosTimer::ChaosTimer()
{
    const auto shipDestroyedAddress = reinterpret_cast<PDWORD>(NakedShipDestroyed);

    const auto offset = RelOfs("server.dll", AddressTable::ShipDestroyedFunction);
    MemUtils::ReadProcMem(offset, &oldShipDestroyed, 4);
    MemUtils::WriteProcMem(offset, &shipDestroyedAddress, 4);

    consumeFireResourcesDetour.Detour(OnConsumeFireResources);
    canFireDetour.Detour(OnCanFire);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ChaosTimer::DelayActiveEffect(ActiveEffect* effect, const float delay)
{
    if (const auto found = activeEffects.find(effect); found != activeEffects.end())
    {
        found->second += delay;
    }
}

void ChaosTimer::ToggleDoubleTime() { doubleTime = !doubleTime; }
bool ChaosTimer::DoubleTimeActive() const { return doubleTime; }
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

float ChaosTimer::GetTimeUntilChaos() const { return Get<ConfigManager>()->chaosSettings.timeBetweenChaos - currentTime; }

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

void ChaosTimer::Update(const float delta)
{
    const auto config = Get<ConfigManager>();

    timeSinceLastUpdate -= delta;
    if (timeSinceLastUpdate < 0.f && config->discordSettings.timerType == DiscordSettings::TimerType::TimeUntilChaos)
    {
        timeSinceLastUpdate = 15.f;
        const auto lastEffectTimestamp = static_cast<int64>(TimeUtils::UnixTime<std::chrono::seconds>());
        const auto nextEffectTimestamp = lastEffectTimestamp + static_cast<int64>(GetTimeUntilChaos());
        Get<DiscordManager>()->SetActivity("Dreading the next act of Chaos", lastEffectTimestamp, nextEffectTimestamp);
    }

    // If they don't have a ship lets not do chaos (aka are they in space?)
    const auto currentShip = Utils::GetCShip();

    if (!currentShip)
    {
        uint base;
        pub::Player::GetBase(1, base);
        // If they are not in a base, and don't have a ship we assume they are in the death screen
        if (!base)
        {
            OnSystemUnload();
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

    if (config->chaosSettings.enable && !config->chaosSettings.enableTwitchVoting && currentTime > config->chaosSettings.timeBetweenChaos)
    {
        TriggerChaos();

        if (doubleTime)
        {
            TriggerChaos();
        }
    }

    ImGuiManager::SetProgressBarPercentage(currentTime / config->chaosSettings.timeBetweenChaos);

    // Trigger chaos updates
    auto effect = std::begin(activeEffects);

    while (effect != std::end(activeEffects))
    {
        if (auto& info = effect->first->GetEffectInfo(); info.isTimed)
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

const std::unordered_map<ActiveEffect*, float>& ChaosTimer::GetActiveEffects() { return Get<ChaosTimer>()->activeEffects; }
const std::vector<PersistentEffect*>& ChaosTimer::GetActivePersistentEffects() { return Get<ChaosTimer>()->persistentEffects; }
void ChaosTimer::EndEffectPrematurely(const ActiveEffect* effect)
{
    for (auto& all = Get<ChaosTimer>()->activeEffects; auto& iter : all)
    {
        if (iter.first == effect)
        {
            iter.second = 0.0f;
        }
    }
}
