#pragma once

#include "Components/Component.hpp"
#include "Effects/ActiveEffect.hpp"
#include "Effects/PersistentEffect.hpp"
#include "FLCore/Common/CEquip/CAttachedEquip/CELauncher/CEGun.hpp"
#include "Utils/Detour.hpp"

#include <unordered_map>

class OnSystemStatusChange;
class PatchNotes;
class ChaosTimer final : public Component
{
        friend OnSystemStatusChange;

        using ConsumeFireResourcesType = void(__fastcall*)(CELauncher* launcher);
        using CanFireType = FireResult(__fastcall*)(CEGun* gun, void* edx, const Vector& target);
        inline static FunctionDetour<ConsumeFireResourcesType> consumeFireResourcesDetour{ reinterpret_cast<ConsumeFireResourcesType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?ConsumeFireResources@CELauncher@@UAEXXZ")) };
        inline static FunctionDetour<CanFireType> canFireDetour{ reinterpret_cast<CanFireType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?CanFire@CEGun@@MBE?AW4FireResult@@ABVVector@@@Z")) };

        float currentTime = 0.0f;
        float modifiers = 1.0f;
        bool doubleTime = false;
        CShip* lastPlayerShip = nullptr;

        float timeSinceLastUpdate = 15.f;

        static void __stdcall ShipDestroyed(DamageList* dmgList, DWORD* ecx, uint kill);

        static void OnMunitionHit(EqObj* hitObject, MunitionImpactData* impact, DamageList* dmgList, bool after);
        static bool OnExplosion(EqObj* hitObject, ExplosionDamageEvent* explosion, DamageList* dmgList);
        static void OnSystemUnload();
        static void OnJumpInComplete();
        static FireResult __fastcall OnCanFire(CEGun* gun, void* edx, const Vector& target);
        static void __fastcall OnConsumeFireResources(CELauncher* launcher);

        static void PlayBadEffect();
        static void PlayEffectSkip();
        static void PlayNextEffect();
        ActiveEffect* SelectEffect();

        std::unordered_map<ActiveEffect*, float> activeEffects;
        std::vector<PersistentEffect*> persistentEffects;

    public:
        // Events

        static uint OnSoundEffect(uint hash);

        // Effect Management

        ChaosTimer(const ChaosTimer& other) = delete;
        ChaosTimer();
        void DelayActiveEffect(ActiveEffect* effect, float delay);
        void TriggerChaos(ActiveEffect* effect = nullptr);
        void ToggleDoubleTime();
        [[nodiscard]]
        bool DoubleTimeActive() const;
        void AdjustModifier(float modifier);
        void Update(float delta);
        void FrameUpdate(float delta);
        void InitEffects();
        static const std::unordered_map<ActiveEffect*, float>& GetActiveEffects();
        static const std::vector<PersistentEffect*>& GetActivePersistentEffects();
        static void EndEffectPrematurely(const ActiveEffect* effect);

        [[nodiscard]]
        float GetTimeUntilChaos() const;

        std::vector<ActiveEffect*> GetNextEffects(int count = 3);
};
