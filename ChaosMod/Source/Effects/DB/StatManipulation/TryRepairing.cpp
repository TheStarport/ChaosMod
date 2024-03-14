#include "PCH.hpp"

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
        explicit TryRepairing(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(TryRepairing, {
    .effectName = "Try Repairing",
    .description = "Laz Giveth, and Laz taketh away. Your nanobots looked tasty so I ate them.",
    .category = EffectType::StatManipulation,
    .timingModifier = 0.0f,
    .isTimed = false,
});
