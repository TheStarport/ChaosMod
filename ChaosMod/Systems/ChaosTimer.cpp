#include "PCH.hpp"

#include "ChaosTimer.hpp"

#include "ConfigManager.hpp"
#include "Effects/ActiveEffect.hpp"
#include "Effects/DB/Audio/BossMusic.hpp"
#include "Effects/DB/Interface/LGBTUI.hpp"
#include "UiManager.hpp"

#include <magic_enum.hpp>

void ChaosTimer::PlayBadEffect() { pub::Audio::PlaySoundEffect(1, CreateID("news_vendor_open")); }
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

    std::random_device r;
    std::default_random_engine engine(r());
    std::uniform_int_distribution<uint> dist(0x0, possibleEffects.size() - 1);

    for (uint attempts = 0; attempts < 5; attempts++)
    {
        const auto val = dist(engine);
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

void ChaosTimer::TriggerChaos()
{
    uint* aaP = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C - 28);
    uint* aaa = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C - 24);
    uint* aa23 = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C - 20);
    uint* a = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C - 16);
    uint* b = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C - 12);
    uint* c = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C - 8);
    uint* d = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C - 4);
    uint* e = (uint*)(DWORD(GetModuleHandleA("freelancer.exe")) + 0x27995C);

    const auto effect = SelectEffect();
    if (!effect)
    {
        PlayBadEffect();
        return;
    }

    // Too many active effects
    if (activeEffects.size() >= ConfigManager::i()->totalAllowedConcurrentEffects)
    {
        // Clean out those without timers
        std::erase_if(activeEffects, [](const auto& active) { return active.first->GetEffectInfo().isTimed; });
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
    const float timing = info.isTimed ? info.timingModifier * ConfigManager::i()->defaultEffectDuration : 30.0f;

    activeEffects[effect] = timing;
    PlayNextEffect();
}

void ChaosTimer::Update(const float delta)
{
    currentTime += delta;

    const float nextTime = ConfigManager::i()->timeBetweenChaos * modifiers;
    if (currentTime > nextTime)
    {
        currentTime = 0.0f;
        TriggerChaos();
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
            if (info.isTimed)
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
}

const std::unordered_map<ActiveEffect*, float>& ChaosTimer::GetActiveEffects() { return i()->activeEffects; }

void ChaosTimer::RegisterAllEffects()
{
#define Ef(T) ActiveEffect::RegisterEffect<T>()
    Ef(Lgbtui);
    Ef(BossMusic);
#undef Ef
}
