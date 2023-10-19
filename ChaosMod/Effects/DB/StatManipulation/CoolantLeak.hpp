#pragma once
#include "Effects/ActiveEffect.hpp"

class CoolantLeak final : public ActiveEffect
{
        static constexpr float fps = 1.0f / 60.0f;
        void Update(float delta) override
        {
            CShip* ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            CEquipTraverser tr(Power);
            const CEquip* equip;
            float powerIncrease = 0.0f;
            while ((equip = GetEquipManager((ship))->Traverse(tr)))
            {
                const auto power = static_cast<const CEPower*>(equip);
                powerIncrease += power->GetChargeRate() * fps;
            }

            // Set the regen of the ship to 40% of what it should be
            const auto currentPower = ship->get_power();
            ship->set_power(currentPower - (powerIncrease * 0.4f));
        }

    public:
        DefaultEffectInfo("Coolant Leak", EffectType::StatManipulation);
};
