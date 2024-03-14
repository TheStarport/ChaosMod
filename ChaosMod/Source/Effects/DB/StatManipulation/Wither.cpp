#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Wither final : public ActiveEffect
{
        using RefireDelayElapsedType = bool(__fastcall*)(CELauncher*);

        static bool __fastcall RefireDelayElapsed(CELauncher* launcher)
        {
            return *(reinterpret_cast<float*>(launcher) + 20) >=
                   static_cast<double>(*reinterpret_cast<float*>(*(reinterpret_cast<DWORD*>(launcher) + 3) + 144)) * 2;
        }

        inline static FunctionDetour<RefireDelayElapsedType> refireDelayElapsedDetour{ reinterpret_cast<RefireDelayElapsedType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?RefireDelayElapsed@CELauncher@@MBE_NXZ")) };

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_wither"));
            refireDelayElapsedDetour.Detour(RefireDelayElapsed);
        }
        void End() override { refireDelayElapsedDetour.UnDetour(); }

    public:
        explicit Wither(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Wither, {
    .effectName = "Wither",
    .description = "Nasus ages a champion, slowing them by 35%, increasing to 47 / 59 / 71 / 83 / 95% over 5 seconds. Their attack speed is also reduced by 75% of the slow.",
    .category = EffectType::StatManipulation,
    .timingModifier = 1.5f,
    .exclusion = EffectExclusion::RefireDelay
});