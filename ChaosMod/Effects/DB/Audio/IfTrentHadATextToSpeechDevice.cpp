// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

#include <sapi.h>

class IfTrentHadATextToSpeechDevice final : public ActiveEffect
{
        std::string ttsLinesRaw =
#include "TTS.txt"
            ;
        std::vector<std::string> ttsLines{};

        std::jthread ttsThread{};
        void Begin() override
        {
            const uint index = Random::i()->Uniform(0u, ttsLines.size() - 1);

            auto line = ttsLines[index];

            ttsThread = std::jthread(
                [line]
                {
                    const std::wstring wide = StringUtils::stows(line);
                    ISpVoice *pVoice = nullptr;
                    if (const HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice); SUCCEEDED(hr))
                    {
                        pVoice->Speak(wide.c_str(), 0, nullptr);
                        pVoice->Release();
                        pVoice = nullptr;
                    }
                });
        }

        void Cleanup() override
        {
            if (ttsThread.joinable())
            {
                ttsThread.request_stop();
                ttsThread.join();
            }
        }

    public:
        IfTrentHadATextToSpeechDevice(const EffectInfo &info) : ActiveEffect(info)
        {
            for (auto line : StringUtils::GetParams(ttsLinesRaw, '#'))
            {
                ttsLines.emplace_back(line);
            }
        }
};

// clang-format off
SetupEffect(IfTrentHadATextToSpeechDevice, {
    .effectName = "If Trent Had A Text To Speech Device",
    .description = "Plays a random line via the system's text-to-speech.",
    .category = EffectType::Audio,
});