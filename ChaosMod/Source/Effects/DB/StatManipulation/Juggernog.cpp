#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Juggernog final : public ActiveEffect
{
        void OnApplyDamage(uint hitSpaceObj, DamageList* dmgList, DamageEntry& dmgEntry) override
        {
            float currentShield = 0.f;
            float maxShield = 0.f;
            bool shieldsUp = false;
            pub::SpaceObj::GetShieldHealth(hitSpaceObj, currentShield, maxShield, shieldsUp);
            if (const auto ship = Utils::GetCShip(); ship && ship->id == hitSpaceObj)
            {
                if (shieldsUp)
                {
                    dmgEntry.health += (currentShield - dmgEntry.health) * 0.5f;
                }
                else
                {
                    dmgEntry.health += (ship->hitPoints - dmgEntry.health) * 0.5f;
                }
            }
        }

        void Begin() override { pub::Audio::PlaySoundEffect(1, CreateID("chaos_juggernog")); }

    public:
        explicit Juggernog(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Juggernog, {
    .effectName = "Juggernog",
    .description = "It makes you big and strong, just as advertised!",
    .category = EffectType::StatManipulation,
});