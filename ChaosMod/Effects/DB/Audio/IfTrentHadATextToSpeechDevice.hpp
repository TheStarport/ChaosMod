// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

#include "Effects/MemoryEffect.hpp"

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
            std::random_device r;
            std::default_random_engine engine(r());
            std::uniform_int_distribution<uint> dist(0x0, ttsLines.size() - 1);

            auto line = ttsLines[dist(engine)];

            ttsThread = std::jthread(
                [line]
                {
                    const std::wstring wide = Utils::String::stows(line);
                    ISpVoice *pVoice = nullptr;
                    if (const HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice); SUCCEEDED(hr))
                    {
                        pVoice->Speak(wide.c_str(), 0, nullptr);
                        pVoice->Release();
                        pVoice = nullptr;
                    }
                });
        }

    public:
        IfTrentHadATextToSpeechDevice() { ttsLines = Utils::String::Split(ttsLinesRaw, '#'); }

        DefEffectInfo("If Trent Had A Text To Speech Device", 0.0f, EffectType::Audio);
};
