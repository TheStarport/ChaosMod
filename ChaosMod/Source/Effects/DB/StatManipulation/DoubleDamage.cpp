#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class DoubleDamage final : public ActiveEffect
{
        void OnMunitionHitAfter(EqObj* hitObject, MunitionImpactData* data, DamageList* dmgList) override
        {
            // Double the amount of damage that would be applied
            float currentShield = 0.f;
            float maxShield = 0.f;
            bool shieldsUp = false;
            pub::SpaceObj::GetShieldHealth(hitObject->get_id(), currentShield, maxShield, shieldsUp);

            auto& entry = dmgList->damageEntries.front();
            if (shieldsUp)
            {
                entry.health -= currentShield - entry.health;
            }
            else
            {
                entry.health -= hitObject->ceqobj()->hitPoints - entry.health;
            }
        }

    public:
        explicit DoubleDamage(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DoubleDamage, {
    .effectName = "Double Damage",
    .description = "Wouldn't it be great if you did twice as much damage? Well now you can. NPCs might also hurt though...",
    .category = EffectType::StatManipulation,
});