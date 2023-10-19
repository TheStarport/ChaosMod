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
        AbsoluteEffectInfo("Self Destruct", EffectType::Misc, 4.0f);
};
