#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class DeadlyMissiles final : public ActiveEffect
{
        bool OnExplosion(EqObj* hitObject, ExplosionDamageEvent* explosion, DamageList* dmgList) override
        {
            auto* cEq = hitObject->ceqobj();
            if (auto* shield = reinterpret_cast<CEShield*>(cEq->equipManager.FindFirst(static_cast<unsigned int>(EquipmentClass::Shield)));
                shield->IsFunctioning())
            {
                const auto dmg = (explosion->explosionArchetype->energyDamage + explosion->explosionArchetype->hullDamage * 0.5f) * 3.f;
                hitObject->damage_shield_direct(shield, dmg, dmgList);
                if (shield->hitPts < dmg)
                {
                    // Bleed through
                    hitObject->damage_hull(dmg - shield->hitPts, dmgList);
                }
            }
            else
            {
                CArchGrpTraverser traverser;
                CArchGroup* group;

                while ((group = cEq->archGroupManager.Traverse(traverser)))
                {
                    hitObject->damage_col_grp(group, explosion->explosionArchetype->hullDamage * 1.75f, dmgList);
                }

                hitObject->damage_hull(explosion->explosionArchetype->hullDamage * 4.0f, dmgList);
            }

            return false;
        }

    public:
        explicit DeadlyMissiles(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DeadlyMissiles, {
    .effectName = "Deadly Missiles",
    .description = "We all know that missiles are pretty deadly in Freelancer, but now they are extra deadly! (Includes mines too, but who uses them anyways?)",
    .category = EffectType::StatManipulation,
});