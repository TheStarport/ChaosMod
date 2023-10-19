#pragma once
#include "Effects/ActiveEffect.hpp"

class Wanted final : public ActiveEffect
{
        void Begin() override
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

                                            int reputation;
                                            pub::Player::GetRep(1, reputation);
                                            int npcReputation;
                                            pub::SpaceObj::GetRep(npc->id, npcReputation);
                                            pub::Reputation::SetAttitude(npcReputation, reputation, -0.9f);
                                        });
        }

    public:
        OneShotEffectInfo("WANTED", EffectType::Npc);
};
