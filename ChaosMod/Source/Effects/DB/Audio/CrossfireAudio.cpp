#include "PCH.hpp"

#include "Effects/MemoryEffect.hpp"

#include <MMREG.h>
#include <MSAcm.h>
#pragma comment(lib, "Msacm32.lib")

class CrossfireAudio final : public ActiveEffect
{
        float* audioVoice = nullptr;
        float* audioMusic = nullptr;
        float* audioAmbient = nullptr;
        float* audioSound = nullptr;
        float* audioInterface = nullptr;
        float originalVoiceVolume = 0.0f;
        float originalSfxVolume = 0.0f;
        float originalMusicVolume = 0.0f;
        inline static bool active = false;

        static MMRESULT ACMAPI SoundStreamConvertDetour(const HACMSTREAM has, const LPACMSTREAMHEADER header, const DWORD convert)
        {
            const auto res = acmStreamConvert(has, header, convert);

            if (!active || res != 0)
            {
                return res;
            }

            // Make the audio awful
            const auto samples = reinterpret_cast<int16_t*>(header->pbDst);
            for (int i = 0; i < header->cbDstLength / 2; i++)
            {
                float s = samples[i] * 100.0;
                if (s < -32768)
                {
                    s = -32768;
                }
                if (s > 32767)
                {
                    s = 32767;
                }

                samples[i] = static_cast<int16_t>(s / 8.0);
            }

            return res;
        }

        void GetCurrentFreelancerOptions()
        {
            const DWORD optionsTable = *reinterpret_cast<DWORD*>(0x67ED04);
            const auto unk = reinterpret_cast<DWORD*>(optionsTable);

            for (int i = *reinterpret_cast<DWORD*>(optionsTable); reinterpret_cast<DWORD*>(i) != unk; i = *reinterpret_cast<DWORD*>(i))
            {
                const DWORD kvp = *reinterpret_cast<DWORD*>(i + 8);
                if (const auto variableName = reinterpret_cast<const char*>(*reinterpret_cast<DWORD*>(kvp + 4)); strcmp("audio_voice", variableName) == 0)
                {
                    audioVoice = reinterpret_cast<float*>(kvp + 20);
                }
                else if (strcmp("audio_sfx", variableName) == 0)
                {
                    audioSound = reinterpret_cast<float*>(kvp + 20);
                }
                else if (strcmp("audio_interface", variableName) == 0)
                {
                    audioInterface = reinterpret_cast<float*>(kvp + 20);
                }
                else if (strcmp("audio_music", variableName) == 0)
                {
                    audioMusic = reinterpret_cast<float*>(kvp + 20);
                }
                else if (strcmp("audio_ambient", variableName) == 0)
                {
                    audioAmbient = reinterpret_cast<float*>(kvp + 20);
                }
            }
        }

        void Begin() override
        {
            active = true;

            GetCurrentFreelancerOptions();
            originalVoiceVolume = *audioVoice;
            originalSfxVolume = *audioSound;
            originalMusicVolume = *audioMusic;
        };

        void Update(float delta) override
        {
            GetCurrentFreelancerOptions();
            // Force the voice volume to be on and LOUD
            *audioVoice = std::clamp(*audioVoice, 0.70f, 1.0f);

            // Force the rest to be louder, but not overwhelming
            *audioMusic = std::clamp(*audioMusic, 0.35f, 1.0f);
            *audioSound = std::clamp(*audioSound, 0.35f, 1.0f);
            *audioAmbient = *audioMusic;
            *audioInterface = *audioSound;
        }

        void End() override
        {
            active = false;

            *audioVoice = originalVoiceVolume;
            *audioSound = originalSfxVolume;
            *audioInterface = originalSfxVolume;
            *audioMusic = originalMusicVolume;
            *audioAmbient = originalMusicVolume;
        };

        void Init() override
        {
            MemUtils::PatchCallAddr(GetModuleHandleA("soundmanager.dll"), 0x87DF, SoundStreamConvertDetour);
            MemUtils::PatchCallAddr(GetModuleHandleA("soundstreamer.dll"), 0x2296, SoundStreamConvertDetour);
        }

    public:
        explicit CrossfireAudio(const EffectInfo& info) : ActiveEffect(info) {}
};

// clang-format off
SetupEffect(CrossfireAudio, {
    .effectName = "Crossfire Audio",
    .description = "We did our best to capture the quality of voice acting. WARNING: Can be a bit loud!",
    .category = EffectType::Audio,
    .continueOnBases = true,
});