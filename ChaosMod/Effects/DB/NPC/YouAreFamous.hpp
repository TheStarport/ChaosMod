#pragma once
#include "Cardamine.hpp"
#include "Effects/ActiveEffect.hpp"

class YouAreFamous final : public ActiveEffect
{
        void Update(float delta) override
        {
            auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            Utils::ForEachShip(
                [ship](auto npc)
                {
                    if (ship == npc)
                    {
                        return;
                    }

                    auto newOp = pub::AI::DirectiveGotoOp();
                    newOp.iGotoType = 0;
                    newOp.ship_moves1 = true;
                    newOp.ship_moves2 = true;
                    newOp.goto_cruise = true;
                    newOp.iTargetID = ship->iID;
                    newOp.fRange = 1.0f;
                    newOp.fThrust = -1.0f;
                    pub::AI::SubmitDirective(npc->iID, &newOp);
                });
        }

    public:
        DefEffectInfo("You Are Famous", 1.0f, EffectType::Npc);
};
