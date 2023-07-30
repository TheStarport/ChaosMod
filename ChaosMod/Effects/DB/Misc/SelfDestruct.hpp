#pragma once
#include "Effects/ActiveEffect.hpp"

class SelfDestruct final : public ActiveEffect
{
        void End() override
        {
            if (const CShip* ship = Utils::GetCShip())
            {
                pub::SpaceObj::Destroy(ship->id, DestroyType::Fuse);
            }
        }

    public:
        DefEffectInfo("Self Destruct", 0.1f, EffectType::Misc);
};
