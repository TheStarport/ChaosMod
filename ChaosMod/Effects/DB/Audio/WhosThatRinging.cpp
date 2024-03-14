// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class WhosThatRinging final : public ActiveEffect
{
        std::map<uint, float> soundDurationMap = {
            {CreateID("chaos_jingle_discord_halloween"),  5.f},
            {CreateID("chaos_jingle_discord_halloween"), 18.f},
            {     CreateID("chaos_jingle_discord_xmas"), 32.f},
            {            CreateID("chaos_jingle_skype"),  7.f},
            {            CreateID("chaos_jingle_teams"), 12.f}
        };

        uint currentEffect = 0;
        float effectDuration = 0.f;
        float timeUntilNextEffect = 0.f;
        void Begin() override
        {
            const auto index = Get<Random>()->Uniform(0u, soundDurationMap.size() - 1);
            auto pair = soundDurationMap.begin();
            std::advance(pair, index);

            currentEffect = pair->first;
            effectDuration = pair->second;
        }

        void Update(const float delta) override
        {
            timeUntilNextEffect -= delta;
            if (timeUntilNextEffect < 0.0f)
            {
                timeUntilNextEffect = effectDuration;
                pub::Audio::PlaySoundEffect(1, currentEffect);
            }
        }

    public:
        explicit WhosThatRinging(const EffectInfo& info) : ActiveEffect(info) {}
};

// clang-format off
SetupEffect(WhosThatRinging, {
    .effectName = "Who's That Ringing?",
    .description = "You should pick up the phone. Not polite to leave someone ringing forever and ever...",
    .category = EffectType::Audio,
    .fixedTimeOverride = 70.f
});