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

            Utils::ForEachShip(
                [ship](auto npc)
                {
                    if (ship == npc)
                    {
                        return;
                    }

                    int reputation;
                    pub::Player::GetRep(1, reputation);
                    int npcReputation;
                    pub::SpaceObj::GetRep(npc->iID, npcReputation);
                    pub::Reputation::SetAttitude(npcReputation, reputation, -0.9f);
                });
        }

    public:
        DefEffectInfo("WANTED", 0.0f, EffectType::Npc);
};
