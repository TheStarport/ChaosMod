#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class DodgeThis final : public ActiveEffect
{
        inline static std::array<uint, 6> sounds = {
            CreateID("chaos_bat_1"), CreateID("chaos_bat_2"), CreateID("chaos_bat_3"),
            CreateID("chaos_bat_4"), CreateID("chaos_bat_5"), CreateID("chaos_bat_6"),
        };

        void OnApplyDamageAfter(uint hitSpaceObj, DamageList* dmgList, const DamageEntry& dmgEntry) override
        {
            pub::Audio::PlaySoundEffect(0, sounds[Get<Random>()->Uniform(0u, sounds.size() - 1)]);
        }

    public:
        explicit DodgeThis(const EffectInfo& info) : ActiveEffect(info) {}
};

// clang-format off
SetupEffect(DodgeThis, {
    .effectName = "Dodge This",
    .description = "Plays a comedic hit sound effect whenever any damage is applied.",
    .category = EffectType::Audio,
});