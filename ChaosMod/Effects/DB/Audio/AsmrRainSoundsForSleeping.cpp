// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

#include "miniaudio.h"

class AsmrRainSoundsForSleeping final : public ActiveEffect
{
        ma_engine engine;
        ma_sound sound;

        void Begin() override
        {
            ma_sound_seek_to_pcm_frame(&sound, 0);
            ma_sound_start(&sound);
        }

        void End() override { ma_sound_stop(&sound); }

        void Cleanup() override
        {
            ma_sound_uninit(&sound);
            ma_engine_uninit(&engine);
        }

        void Init() override
        {
            ma_engine_init(nullptr, &engine);
            ma_sound_init_from_file(&engine, "../DATA/CHAOS/SOUNDS/chaos_rain_sounds.wav", 0, nullptr, nullptr, &sound);
            ma_sound_set_volume(&sound, 0.2f);
        }

    public:
        explicit AsmrRainSoundsForSleeping(const EffectInfo& info) : ActiveEffect(info) {}
};

// clang-format off
SetupEffect(AsmrRainSoundsForSleeping, {
    .effectName = "ASMR Rain Sounds For Sleeping",
    .description = "The best rain sounds with thunder will help you relax, quickly fall asleep and have beautiful dreams",
    .category = EffectType::Audio,
    .timingModifier = 3.0f
});