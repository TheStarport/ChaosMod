#pragma once

#include "Effects/ActiveEffect.hpp"

class IDoNotSpeakMorseCode final : public ActiveEffect
{
        float time = 3.0f;

        static void Play() { pub::Audio::PlaySoundEffect(1, CreateID("ui_typing_characters")); }

        void Begin() override
        {
            time = 3.0f;
            Play();
        }

        void Update(const float delta) override
        {
            time -= delta;
            if (time < 0.0f)
            {
                time = 3.0f;
                Play();
            }
        }

    public:
        DefaultEffectInfo("I Do Not Speak Morse Code", EffectType::Audio);
};
