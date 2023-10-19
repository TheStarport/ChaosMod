// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once

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
        DefaultEffectInfo("All The News That's Fit For You", EffectType::Audio);
};
