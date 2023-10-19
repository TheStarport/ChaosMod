#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/KeyManager.hpp"

class AssumingDirectControl final : public ActiveEffect
{
        CSolar* solar = nullptr;
        uint solarId = 0;
        float timer = 1.0f;

        void Begin() override
        {
            timer = 1.0f;
            solar = nullptr;
            solarId = 0;

            const auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            Utils::ForEachObject<CSolar>(CObject::Class::CSOLAR_OBJECT,
                                         [ship, this](auto next)
                                         {
                                             if (next->get_dock_target() == 0)
                                             {
                                                 return;
                                             }

                                             if (!solar)
                                             {
                                                 solar = next;
                                             }
                                             else
                                             {
                                                 const auto previousLength = glm::length(ship->position - solar->position);
                                                 const auto nextLength = glm::length(ship->position - next->position);
                                                 if (std::abs(nextLength) < std::abs(previousLength))
                                                 {
                                                     solar = next;
                                                 }
                                             }
                                         });

            if (solar)
            {
                if (Random::i()->Uniform(0u, 1u))
                {
                    pub::Audio::PlaySoundEffect(1, CreateID("chaos_assuming_control_01"));
                }
                else
                {
                    pub::Audio::PlaySoundEffect(1, CreateID("chaos_assuming_control_02"));
                }

                solarId = solar->id;
                KeyManager::BlockPlayerInput(true);

                pub::AI::DirectiveGotoOp op;
                op.range = 500.0f;
                op.targetId = solar->id;
                op.shipMoves = true;
                op.shipMoves2 = true;
                op.gotoType = 0;
                op.goToCruise = true;

                pub::AI::SubmitDirective(ship->id, &op);
            }
        }

        void Update(float delta) override
        {
            timer -= delta;
            if (timer > 0.0f)
            {
                return;
            }

            timer = 1.0f;
            auto ship = Utils::GetCShip();
            solar = static_cast<CSolar*>(CObject::Find(solarId, CObject::Class::CSOLAR_OBJECT));
            if (!solar || !ship)
            {
                return;
            }

            if (const auto distance = glm::length(ship->position - solar->position); std::abs(distance) > 500.0f)
            {
                return;
            }

            timer = 1000.0f;
            pub::AI::DirectiveDockOp op;
            op.DockSpaceObj = solar->id;
            op.x14 = 0xbb83;
            op.x18 = -1;
            op.x20 = 200; // 200
            op.x24 = 500; // 500
            pub::AI::SubmitDirective(ship->id, &op);
        }

        void End() override { KeyManager::BlockPlayerInput(false); }

        void OnShipDestroyed(DamageList* dmgList, CShip* ship) override
        {
            if (ship == Utils::GetCShip())
            {
                End();
            }
        }

    public:
        DefaultEffectInfo("Assuming Direct Control", EffectType::Movement);
};
