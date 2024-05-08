#include "Components/ReshadeManager.hpp"
#include "CoreComponents/ChaosTimer.hpp"
#include "Effects/ActiveEffect.hpp"

class CatJam final : public ActiveEffect
{
    std::array<std::pair<uint, float>, 9> music = {
        std::pair{ CreateID("music_bar_br01"), 72.f },
        { CreateID("music_bar_ku02"), 88.f },
        { CreateID("music_bar_rh01"), 55.f },
        { CreateID("music_m13_hypergate_scene"), 60.f },
        { CreateID("music_no_lair_battle"), 52.f },
        { CreateID("music_victory_long"), 60.f },
        { CreateID("music_kyushu"), 23.f },
        { CreateID("music_friendly_encourage"), 38.f },
        { CreateID("music_race_loop"), 55.f },
    };
    void Begin() override
    {
        auto audio = music[Get<Random>()->Uniform(0u, music.size() - 1)];
        pub::Audio::Tryptich tryptich =
        {
            0,
            0,
            0,
            audio.first
        };

        pub::Audio::SetMusic(1, tryptich);

        Get<ChaosTimer>()->DelayActiveEffect(this, audio.second);

        Get<ReshadeManager>()->ToggleTechnique("Flipbook.fx", "Flipbook", true);
    };

    void End() override
    {
        Get<ReshadeManager>()->ToggleTechnique("Flipbook.fx", "Flipbook", false);
    }

    public:
    explicit CatJam(const EffectInfo& info) : ActiveEffect(info)
    {
    }
};

// clang-format off
SetupEffect(CatJam, {
    .effectName = "Cat Jam",
    .description = "Cat loves the music, and so should you.",
    .category = EffectType::Audio,
    .fixedTimeOverride = 0.01f // This gets extended
});