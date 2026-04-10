// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "Effects/ActiveEffect.hpp"
#include "FLCore/FLCoreServer.h"

class AllTheNewsThatsFitForYou final : public ActiveEffect
{
        float time = 10.0f;
        void Update(const float delta) override
        {
            time -= delta;
            if (time < 0.0f)
            {
                time = 10.0f;
                pub::Audio::PlaySoundEffect(1, CreateID("news_vendor_open"));
            }
        }

    public:
        explicit AllTheNewsThatsFitForYou(const EffectInfo& info) : ActiveEffect(info) {}
};

// clang-format off
SetupEffect(AllTheNewsThatsFitForYou, {
    .effectName = "All The News That's Fit For You",
    .description = "Every 10 seconds the news vendor music will play. I hope you like the news.",
    .category = EffectType::Audio,
});