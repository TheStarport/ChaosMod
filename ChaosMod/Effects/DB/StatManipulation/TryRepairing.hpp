#pragma once
#include "Effects/ActiveEffect.hpp"

class TryRepairing final : public ActiveEffect
{
        void Begin() override
        {
            EQ_ITEM* head = *(EQ_ITEM**)(Players.data + 0x27C);
            for (EQ_ITEM* eq = head->next; eq != head; eq = eq->next)
            {
                if (eq->iGoodID == CreateID("ge_s_repair_01"))
                {
                    pub::Player::RemoveCargo(1, eq->sID, eq->iCount);
                    return;
                }
            }
        }

    public:
        DefEffectInfo("Try Repairing", 0.0f, EffectType::StatManipulation);
};
