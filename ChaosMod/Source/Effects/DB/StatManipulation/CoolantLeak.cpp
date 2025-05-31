#include "PCH.hpp"

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

            CEquipTraverser tr(static_cast<int>(EquipmentClass::Power));
            const CEquip* equip;
            float powerIncrease = 0.0f;
            while ((equip = ship->equipManager.Traverse(tr)))
            {
                const auto power = static_cast<const CEPower*>(equip);
                powerIncrease += power->GetChargeRate() * fps;
            }

            // Set the regen of the ship to 40% of what it should be
            const auto currentPower = ship->get_power();
            ship->set_power(currentPower - (powerIncrease * 0.4f));
        }

        void End() override
        {
            auto ship = Utils::GetCShip();

            if (ship)
            {
                Utils::UnLightFuse(Utils::GetInspect(ship->id), CreateID("chaos_coolant_leak"), 0.0f);
            }
        }

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_coolant_leak"));
            auto ship = Utils::GetCShip();

            Utils::LightFuse(Utils::GetInspect(ship->id), CreateID("chaos_coolant_leak"), 0.0f, 5.0f, 0.0f);
        }

    public:
        explicit CoolantLeak(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(CoolantLeak, {
    .effectName = "Coolant Leak",
    .description = "You've got a coolant leak, and your powercore is suffering for it.",
    .category = EffectType::StatManipulation
});