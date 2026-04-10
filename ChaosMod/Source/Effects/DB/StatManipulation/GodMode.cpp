
#include "Effects/ActiveEffect.hpp"
#include "FLCore/FLCoreServer.h"
class GodMode final : public ActiveEffect
{
        void Begin() override
        {
            const auto ship = Fluf::GetClient()->GetPlayerCShip();
            pub::SpaceObj::SetInvincible(ship->id, true, true, 0);
        }

        void End() override
        {
            if (const auto ship = Fluf::GetClient()->GetPlayerCShip())
            {
                pub::SpaceObj::SetInvincible(ship->id, false, false, 0);
            }
        }

    public:
        explicit GodMode(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(GodMode, {
    .effectName = "God Mode",
    .description = "Oh man that's awesome. Did you find the Konami code?",
    .category = EffectType::StatManipulation
});
