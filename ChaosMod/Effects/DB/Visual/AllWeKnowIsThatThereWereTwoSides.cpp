#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/SystemComponents/MoviePlayer.hpp"

class AllWeKnowIsThatThereWereTwoSides final : public ActiveEffect
{
        void FrameUpdate(float delta) override { MoviePlayer::i()->FrameUpdate(); }

        void Begin() override { MoviePlayer::i()->StartMovie(Movie::OriginalIntro); }

        void End() override { MoviePlayer::i()->StopMovie(); }

        void Init() override { MoviePlayer::i()->RegisterMovie(Movie::OriginalIntro, "../DATA/CHAOS/MOVIES/fl_intro.mp4"); }

    public:
        explicit AllWeKnowIsThatThereWereTwoSides(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(AllWeKnowIsThatThereWereTwoSides, {
    .effectName = "All We Know...",
    .description = "Who doesn't love the opening intro? It has no right to go as hard as it does.",
    .category = EffectType::Visual,
    .fixedTimeOverride = 223.0f,
    .isTimed = true,
    .exclusion = EffectExclusion::Movie
});