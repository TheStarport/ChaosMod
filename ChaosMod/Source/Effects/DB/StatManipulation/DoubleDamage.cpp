#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class DoubleDamage final : public ActiveEffect
{
        void OnApplyDamage(uint hitSpaceObj, DamageList* dmgList, DamageEntry& dmgEntry) override
        {
            const CShip* obj = dynamic_cast<CShip*>(CObject::FindFirst(CObject::CSHIP_OBJECT));
            while (obj)
            {
                if (obj->id == hitSpaceObj)
                {
                    // Double the amount of damage that would be applied
                    float currentShield = 0.f;
                    float maxShield = 0.f;
                    bool shieldsUp = false;
                    pub::SpaceObj::GetShieldHealth(hitSpaceObj, currentShield, maxShield, shieldsUp);

                    if (shieldsUp)
                    {
                        dmgEntry.health -= currentShield - dmgEntry.health;
                    }
                    else
                    {
                        dmgEntry.health -= obj->hitPoints - dmgEntry.health;
                    }

                    break;
                }

                obj = dynamic_cast<CShip*>(CObject::FindNext());
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