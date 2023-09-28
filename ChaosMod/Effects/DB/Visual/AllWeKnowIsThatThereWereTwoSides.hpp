#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/SystemComponents/MoviePlayer.hpp"

class AllWeKnowIsThatThereWereTwoSides final : public ActiveEffect
{
        void FrameUpdate(float delta) override { MoviePlayer::i()->FrameUpdate(); }

        void Begin() override { MoviePlayer::i()->StartMovie(Movie::OriginalIntro); }

        void End() override { MoviePlayer::i()->StopMovie(); }

        void Init() override { MoviePlayer::i()->RegisterMovie(Movie::OriginalIntro, "../DATA/CHAOS/MOVIES/fl_intro.mp4"); }

    public:
        const EffectInfo& GetEffectInfo() override
        {
            static const EffectInfo ef = { "All We Know...", 0.0f, EffectType::Visual, 223.0f, EffectExclusion::Movie };
            return ef;
        };
};
