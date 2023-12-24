#include "PCH.hpp"

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
        explicit YouAreFamous(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(YouAreFamous, {
    .effectName = "You Are Famous",
    .description = "You are famous and everyone wants to be as close to you as possible! Can I get your autograph?",
    .category = EffectType::Npc
});
