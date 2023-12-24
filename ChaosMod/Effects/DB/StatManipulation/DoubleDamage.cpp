#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class DoubleDamage final : public ActiveEffect
{
        void OnApplyDamage(uint hitSpaceObj, DamageList* dmgList, DamageEntry& dmgEntry) override
        {
            dmgList->add_damage_entry(dmgEntry.subObj, dmgEntry.health, dmgEntry.fate);
        }

    public:
        explicit DoubleDamage(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// TODO: Implement double damage properly
