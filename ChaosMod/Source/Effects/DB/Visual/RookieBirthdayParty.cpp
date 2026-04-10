#include "Server/FlufServer.hpp"
#include "Effects/ActiveEffect.hpp"
#include "FLCore/FLCoreServer.h"
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

            auto inspect = static_cast<Ship*>(Fluf::GetFlufServer()->GetObjInspect(ship->id));
            if (inspect)
            {
                inspect->light_fuse(0, CreateID("chaos_confetti_explosion"), 0.0f, 5.0f, 0.0f);
                pub::Audio::PlaySoundEffect(1, CreateID("chaos_confetti_sound"));
            }
        }

    public:
        explicit RookieBirthdayParty(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(RookieBirthdayParty, {
    .effectName = "Rookie's Birthday Party",
    .description = "Just like playing Halo back in the day. Every kill is greeted with an celebration!",
    .category = EffectType::Visual
});
