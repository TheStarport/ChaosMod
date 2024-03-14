#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class IDoNotSpeakMorseCode final : public ActiveEffect
{
        float time = 3.0f;

    private:
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
        explicit IDoNotSpeakMorseCode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(IDoNotSpeakMorseCode, {
    .effectName = "I Do Not Speak Morse Code",
    .description = "Plays the typing sound that you would normally hear when you enter the system",
    .category = EffectType::Audio,
});