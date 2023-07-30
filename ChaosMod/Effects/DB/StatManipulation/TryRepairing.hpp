#pragma once
#include "Effects/ActiveEffect.hpp"

class TryRepairing final : public ActiveEffect
{

        struct EquipmentItem
        {
                EquipmentItem* next;
                uint unk;
                ushort unk1;
                ushort id;
                uint goodId;
                CacheString hardpoint;
                bool mounted;
                char unk2[3];
                float status;
                uint count;
                bool mission;
        };

        void Begin() override
        {
            EquipmentItem* head = *(EquipmentItem**)(Players.data + 0x27C);
            for (EquipmentItem* eq = head->next; eq != head; eq = eq->next)
            {
                if (eq->goodId == CreateID("ge_s_repair_01"))
                {
                    pub::Player::RemoveCargo(1, eq->id, eq->count);
                    return;
                }
            }
        }

    public:
        DefEffectInfo("Try Repairing", 0.0f, EffectType::StatManipulation);
};
