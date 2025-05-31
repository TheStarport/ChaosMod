// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Components/TTS.hpp"
#include "Effects/ActiveEffect.hpp"

class IfTrentHadATextToSpeechDevice final : public ActiveEffect
{
        std::string ttsLinesRaw =
#include "TTS.txt"

            ;
        std::vector<std::string> ttsLines{};

        void Begin() override
        {
            const uint index = Get<Random>()->Uniform(0u, ttsLines.size() - 1);
            const auto line = ttsLines[index];

            Get<TTS>()->Speak(line);
        }

    public:
        IfTrentHadATextToSpeechDevice(const EffectInfo &info) : ActiveEffect(info)
        {
            for (auto line : StringUtils::GetParams(ttsLinesRaw, '#'))
            {
                using namespace std::string_view_literals;
                ttsLines.emplace_back(line.substr(0, line.size() - 1));
            }
        }
};

// clang-format off
SetupEffect(IfTrentHadATextToSpeechDevice, {
    .effectName = "If Trent Had A Text To Speech Device",
    .description = "Plays a random line via the system's text-to-speech.",
    .category = EffectType::Audio,
    .timingModifier = 0.0f,
    .isTimed = false
});