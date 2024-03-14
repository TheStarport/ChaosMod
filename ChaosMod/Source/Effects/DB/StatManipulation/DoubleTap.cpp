#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class DoubleTap final : public ActiveEffect
{
        using RefireDelayElapsedType = bool(__fastcall*)(CELauncher*);

        static bool __fastcall RefireDelayElapsed(CELauncher* launcher)
        {
            return *(reinterpret_cast<float*>(launcher) + 20) >=
                   static_cast<double>(*reinterpret_cast<float*>(*(reinterpret_cast<DWORD*>(launcher) + 3) + 144)) * 0.5f;
        }

        inline static FunctionDetour<RefireDelayElapsedType> refireDelayElapsedDetour{ reinterpret_cast<RefireDelayElapsedType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?RefireDelayElapsed@CELauncher@@MBE_NXZ")) };

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_double_tap"));
            refireDelayElapsedDetour.Detour(RefireDelayElapsed);
        }

        void End() override { refireDelayElapsedDetour.UnDetour(); }

    public:
        explicit DoubleTap(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DoubleTap, {
    .effectName = "Double Tap Root Beer",
    .description = "Double Tap, Double Tap, Double Tap, Double Tap, Double Tap, Double Tap, Root Beer. Root Beer. Who doesn't like the root beer?",
    .category = EffectType::StatManipulation,
    .timingModifier = 1.5f,
    .exclusion = EffectExclusion::RefireDelay
});