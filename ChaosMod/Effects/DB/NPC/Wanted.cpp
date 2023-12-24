#include "PCH.hpp"

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
        explicit Wanted(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Wanted, {
    .effectName = "WANTED",
    .description = "The news got out about what Trent did, and no one is happy. Everyone in the immediate area has become hostile!",
    .category = EffectType::Npc,
    .timingModifier = 0.0f,
    .isTimed = false
});