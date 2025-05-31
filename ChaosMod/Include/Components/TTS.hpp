#pragma once

#include <miniaudio.h>

class TTS final : public Component
{
        std::jthread thread;
        std::atomic_bool generatingComplete;
        std::string generatedFile;
        std::string directory;
        std::vector<std::string> voices;

        float timeUntilAudioPurge = 300;
        ma_engine engine{};
        ma_sound sound{};
        bool alreadyPlaying = false;

        void GenerateAudio(std::string message);
        void PurgeAudioFiles() const;

    public:
        TTS();
        ~TTS() override;
        void Update(float delta);
        void Speak(std::string_view msg);
};
