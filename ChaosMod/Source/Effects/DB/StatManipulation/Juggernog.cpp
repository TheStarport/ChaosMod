#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Juggernog final : public ActiveEffect
{
        void OnMunitionHitAfter(EqObj* hitObject, MunitionImpactData* impact, DamageList* dmgList) override
        {
            const auto id = hitObject->get_id();
            const auto ship = Utils::GetCShip();
            if (!ship || ship->id != id)
            {
                return;
            }

            float currentShield = 0.f;
            float maxShield = 0.f;
            bool shieldsUp = false;
            pub::SpaceObj::GetShieldHealth(id, currentShield, maxShield, shieldsUp);

            auto& entry = dmgList->damageEntries.front();
            if (shieldsUp)
            {
                entry.health += (currentShield - entry.health) * 0.5f;
            }
            else
            {
                entry.health += (ship->hitPoints - entry.health) * 0.5f;
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