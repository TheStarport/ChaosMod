#pragma once

class DoubleDamage final : public ActiveEffect
{
        void OnApplyDamage(uint hitSpaceObj, DamageList* dmgList, DamageEntry& dmgEntry) override
        {
            dmgList->add_damage_entry(dmgEntry.subObj, dmgEntry.health, dmgEntry.fate);
        }

    public:
        DefaultEffectInfo("Double Damage", EffectType::StatManipulation);
};
