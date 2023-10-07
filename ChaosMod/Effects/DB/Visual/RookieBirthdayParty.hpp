#pragma once
#include "Effects/ActiveEffect.hpp"

class RookieBirthdayParty final : public ActiveEffect
{
        void OnShipDestroyed(DamageList* dmgList, CShip* ship) override
        {
            if (dmgList->damageCause != DamageCause::Gun && dmgList->damageCause != DamageCause::Collision &&
                dmgList->damageCause != DamageCause::CruiseDisrupter && dmgList->damageCause != DamageCause::Mine &&
                dmgList->damageCause != DamageCause::MissileTorpedo)
            {
                return;
            }

            if (const auto inspect = Utils::GetInspect(ship->id))
            {
                Utils::LightFuse(inspect, CreateID("chaos_confetti_explosion"), 0.0f, 5.0f, 0.0f);
                pub::Audio::PlaySoundEffect(1, CreateID("chaos_confetti_sound"));
            }
        }

    public:
        DefEffectInfo("Rookie Birthday Party", 1.0f, EffectType::Visual);
};
