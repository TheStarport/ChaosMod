// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "ChaosMod.hpp"
#include "Components/Random.hpp"
#include "Effects/PersistentEffect.hpp"
#include "FLCore/FLCoreServer.h"
#include "Utils/Detour.hpp"

class SadViolin final : public PersistentEffect
{
        using PlayMusicFunction = int (*)(uint client, const pub::Audio::Tryptich& audio);
        inline static std::unique_ptr<FunctionDetour<PlayMusicFunction>> playMusicDetour;
        inline static uint deathMusicHash = CreateID("music_death");

        inline static std::vector<uint> deathReplacements = {
            CreateID("chaos_titanic"), CreateID("chaos_sad_violin"), CreateID("chaos_xp_shutdown"), CreateID("chaos_you_died"), CreateID("chaos_coffin_dance"),
        };

        static int Detour(const uint client, const pub::Audio::Tryptich& audio)
        {
            auto newAudio = audio;
            if (newAudio.overrideMusic == deathMusicHash)
            {
                auto index = Get<Random>()->Uniform(0u, deathReplacements.size() - 1);
                Log(std::format("Picking new death sound index: {}", index));
                newAudio.overrideMusic = deathReplacements[index];
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

        void Init() override { playMusicDetour = std::make_unique<FunctionDetour<PlayMusicFunction>>(pub::Audio::SetMusic); }

    public:
        explicit SadViolin(const EffectInfo& info) : PersistentEffect(info) {}
};

// clang-format off
SetupEffect(SadViolin, {
    .effectName = "Sad Violin",
    .description = "Replaces the death music until game restart.",
    .category = EffectType::Audio,
    .timingModifier = 0.0f,
    .isTimed = false
});