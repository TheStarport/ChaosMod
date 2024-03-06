#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Effects/PersistentEffect.hpp"

class OnHit;
class OnSound;
class PatchNotes;
class ChaosTimer final : public Singleton<ChaosTimer>
{
        friend OnHit;
        friend OnSound;

        inline static FARPROC oldShipDestroyed = nullptr;
        static void __stdcall ShipDestroyed(DamageList* dmgList, DWORD* ecx, uint kill);
        static void NakedShipDestroyed();

        static void OnApplyDamage(uint hitSpaceObj, DamageList* dmgList, DamageEntry& dmgEntry, bool after);
        static uint OnSoundEffect(uint hash);
        using ConsumeFireResourcesType = void(__fastcall*)(CELauncher* launcher);
        static void __fastcall OnConsumeFireResources(CELauncher* launcher);

        inline static FunctionDetour<ConsumeFireResourcesType> consumeFireResourcesDetour{ reinterpret_cast<ConsumeFireResourcesType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?ConsumeFireResources@CELauncher@@UAEXXZ")) };

        float patchTime = 0.0f;
        float currentTime = 0.0f;
        float modifiers = 1.0f;
        bool doubleTime = false;
        CShip* lastPlayerShip = nullptr;

        static void PlayBadEffect();
        static void PlayEffectSkip();
        static void PlayNextEffect();
        ActiveEffect* SelectEffect();

        std::unordered_map<ActiveEffect*, float> activeEffects;
        std::vector<PersistentEffect*> persistentEffects;
        PatchNotes* patchNotes;

    public:
        ChaosTimer();
        ~ChaosTimer();
        void DelayActiveEffect(ActiveEffect* effect, float delay);
        void TriggerChaos(ActiveEffect* effect = nullptr);
        void ToggleDoubleTime();
        bool DoubleTimeActive() const;
        void AdjustModifier(float modifier);
        void Update(float delta);
        void FrameUpdate(float delta);
        void InitEffects();
        static const std::unordered_map<ActiveEffect*, float>& GetActiveEffects();

        [[nodiscard]]
        float GetTimeUntilChaos() const;

        std::vector<ActiveEffect*> GetNextEffects(int count = 3);
};
