#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class FiftyFifty final : public ActiveEffect
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
            const EquipmentItem* head = *(EquipmentItem**)(Players.data + 0x27C);
            const auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            const uint nanoHash = CreateID("ge_s_repair_01");
            const uint shieldHash = CreateID("ge_s_battery_01");

            // 50/50 chance of giving or taking away regens
            if (Random::i()->Uniform(0u, 1u))
            {
                for (EquipmentItem* eq = head->next; eq != head; eq = eq->next)
                {
                    if (eq->goodId == nanoHash || eq->goodId == shieldHash)
                    {
                        pub::Player::RemoveCargo(1, eq->id, eq->count);
                    }
                }
            }
            else
            {
                uint nanobotCount = 0;
                uint shieldCount = 0;
                for (const EquipmentItem* eq = head->next; eq != head; eq = eq->next)
                {
                    if (eq->goodId == nanoHash)
                    {
                        nanobotCount = eq->count;
                    }
                    else if (eq->goodId == shieldHash)
                    {
                        shieldCount = eq->count;
                    }
                }

                const auto arch = ship->shiparch();
                pub::Player::AddCargo(1, nanoHash, arch->maxNanobots - nanobotCount, 1.f, false);
                pub::Player::AddCargo(1, shieldHash, arch->maxShieldBats - shieldCount, 1.f, false);
            }
        }

    public:
        explicit FiftyFifty(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FiftyFifty, {
    .effectName = "50/50",
    .description = "Will you get regens, or will you have them taken away?",
    .category = EffectType::StatManipulation,
    .timingModifier = 0.0f,
    .isTimed = false,
});
