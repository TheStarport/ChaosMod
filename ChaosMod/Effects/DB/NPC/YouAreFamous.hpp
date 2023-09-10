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

            Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT,
                                        [ship](auto npc)
                                        {
                                            if (ship == npc)
                                            {
                                                return;
                                            }

                                            auto newOp = pub::AI::DirectiveGotoOp();
                                            newOp.gotoType = 0;
                                            newOp.shipMoves = true;
                                            newOp.shipMoves2 = true;
                                            newOp.goToCruise = true;
                                            newOp.targetId = ship->id;
                                            newOp.range = 1.0f;
                                            newOp.thrust = -1.0f;
                                            pub::AI::SubmitDirective(npc->id, &newOp);
                                        });
        }

    public:
        DefEffectInfo("You Are Famous", 1.0f, EffectType::Npc);
};
