#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/SystemComponents/MoviePlayer.hpp"

class AllWeKnowIsThatThereWereTwoSides final : public ActiveEffect
{
        void FrameUpdate(float delta) override { MoviePlayer::i()->FrameUpdate(); }

        void Begin() override { MoviePlayer::i()->StartMovie(Movie::OriginalIntro); }

        void End() override { MoviePlayer::i()->StopMovie(); }

        void Init() override { MoviePlayer::i()->RegisterMovie(Movie::OriginalIntro, "../DATA/MOVIES/intro_hd.mp4"); }

    public:
        DefEffectInfoFixed("All We Know...", 223.0f, EffectType::Visual);
};