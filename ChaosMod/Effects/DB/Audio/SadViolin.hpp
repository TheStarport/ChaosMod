// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

#include "Effects/PersistentEffect.hpp"

class SadViolin final : public PersistentEffect
{
        using PlayMusicFunction = int (*)(uint client, const pub::Audio::Tryptich& audio);
        inline static std::unique_ptr<FunctionDetour<PlayMusicFunction>> playMusicDetour;
        inline static uint deathMusicHash = CreateID("music_death");

        inline static std::vector<uint> deathReplacements = {
            CreateID("chaos_death_titanic"), CreateID("chaos_sad_violin"),   CreateID("chaos_xp_shutdown"),
            CreateID("chaos_you_died"),      CreateID("chaos_coffin_dance"),
        };

        static int Detour(const uint client, const pub::Audio::Tryptich& audio)
        {
            auto newAudio = audio;
            if (newAudio.musicId == deathMusicHash)
            {
                auto index = Random::i()->Uniform(0u, deathReplacements.size() - 1);
                Log(std::format("Picking new death sound index: {}", index));
                newAudio.musicId = deathReplacements[index];
            }

            playMusicDetour->UnDetour();
            const auto res = SetMusic(client, newAudio);
            playMusicDetour->Detour(Detour);
            return res;
        }

        void Begin() override
        {
            PersistentEffect::Begin();

            playMusicDetour->Detour(Detour);
        }

        void End() override { playMusicDetour->UnDetour(); }

    public:
        SadViolin() { playMusicDetour = std::make_unique<FunctionDetour<PlayMusicFunction>>(pub::Audio::SetMusic); }

        DefEffectInfo("Sad Violin", 0.0f, EffectType::Audio);
};