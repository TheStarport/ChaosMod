
#include "Effects/ActiveEffect.hpp"
#include "FLCore/Common/Globals.hpp"
#include "FLCore/FLCoreServer.h"

class ThanksIHateIt final : public ActiveEffect
{
        std::vector<uint> possibleCargo;

        void Begin() override
        {
            const auto ship = Fluf::GetClient()->GetPlayerCShip();
            auto remaining = ship->get_cargo_hold_remaining();
            if (remaining < 1.0f)
            {
                return;
            }

            if (possibleCargo.empty())
            {
                for (auto equip = GameData::equipment.begin(); equip != GameData::equipment.end(); ++equip)
                {
                    if (equip->second->get_class_type() == Archetype::ClassType::Commodity && equip->second->volume > 0.5f)
                    {
                        possibleCargo.emplace_back(Arch2Good(equip->second->archId));
                    }
                }
            }

            while (remaining > 1.0f)
            {
                auto cargo = possibleCargo[Get<Random>()->Uniform(0u, possibleCargo.size() - 1)];
                pub::Player::AddCargo(1, cargo, 1, 1.0f, false);
                remaining -= 1.0f;
            }
        }

    public:
        explicit ThanksIHateIt(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(ThanksIHateIt, {
    .effectName = "Thank's I Hate It",
    .description = "Check your cargo hold. I bet this goodie bag is what you always wanted.!",
    .category = EffectType::Misc,
    .timingModifier = 0.0f,
    .isTimed = false
});