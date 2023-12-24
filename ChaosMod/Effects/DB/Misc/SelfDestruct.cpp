#include "PCH.hpp"

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
        explicit SelfDestruct(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SelfDestruct, {
    .effectName = "Self Destruct",
    .description = "You probably shouldn't have pressed that button. Time to die!",
    .category = EffectType::Misc,
    .fixedTimeOverride = 5.0f,
});