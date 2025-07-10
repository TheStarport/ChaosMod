#pragma once
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class CaramellDansen final : public ActiveEffect
{
        std::string originalStarSphere;

        // clang-format off
        std::array<const char*, 7> colors = {
            R"(CHAOS\MODELS\STARSPHERES\chaos_starsphere_plain_red.cmp)",
            R"(CHAOS\MODELS\STARSPHERES\chaos_starsphere_plain_green.cmp)",
            R"(CHAOS\MODELS\STARSPHERES\chaos_starsphere_plain_blue.cmp)",
            R"(CHAOS\MODELS\STARSPHERES\chaos_starsphere_plain_orange.cmp)",
            R"(CHAOS\MODELS\STARSPHERES\chaos_starsphere_plain_pink.cmp)",
            R"(CHAOS\MODELS\STARSPHERES\chaos_starsphere_plain_yellow.cmp)",
            R"(CHAOS\MODELS\STARSPHERES\chaos_starsphere_plain_cyan.cmp)",
        };
        // clang-format on

        FunctionDetour<int (*)(uint, const pub::Audio::Tryptich&)> playMusicDetour = FunctionDetour{ pub::Audio::SetMusic };
        static int SetMusicDetour(unsigned int client, const pub::Audio::Tryptich& audio) { return -4; }

        int currentColor = -1;
        float timer = 0.33333333f;

        void ChangeBackground(bool reset)
        {
            if (currentColor == colors.size() - 1)
            {
                currentColor = -1;
            }

            Utils::SetNewStarSphere(reset ? originalStarSphere.c_str() : colors[++currentColor]);
        }

        void Begin() override
        {
            timer = 1.0f;

            currentColor = -1;
            originalStarSphere = Utils::GetCurrentStarSphere();

            pub::Audio::Tryptich music;
            music.overrideMusic = CreateID("chaos_caramell_dancen");
            pub::Audio::SetMusic(1, music);

            // Stop any new music overrides from kicking in
            if (playMusicDetour.GetOriginalFunc() == SetMusicDetour)
            {
                playMusicDetour.UnDetour();
            }
            playMusicDetour.Detour(SetMusicDetour);
        }

        void Update(float delta) override
        {
            timer -= delta;
            if (timer > 0)
            {
                return;
            }

            ChangeBackground(false);
            timer = 0.333333f;
        }

        void End() override
        {
            ChangeBackground(true);

            playMusicDetour.UnDetour();

            // Restore original music
            constexpr pub::Audio::Tryptich music;
            pub::Audio::SetMusic(1, music);
        }

    public:
        explicit CaramellDansen(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(CaramellDansen, {
    .effectName = "Caramell Lancen",
    .description = "Vi undrar, är ni redo att vara med?",
    .category = EffectType::Visual,
    .fixedTimeOverride = 172.f
});