
#include "Effects/ActiveEffect.hpp"
#include "FLCore/Common/CEquip/CEPower.hpp"
#include "FLCore/FLCoreServer.h"
class CoolantLeak final : public ActiveEffect
{
        static constexpr float fps = 1.0f / 60.0f;
        void Update(float delta) override
        {
            CShip* ship = Fluf::GetClient()->GetPlayerCShip();
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
            auto ship = static_cast<Ship*>(Fluf::GetClient()->GetPlayerIObj());
            if (ship)
            {
                ship->unlight_fuse(CreateID("chaos_coolant_leak"), 0, 0.0f);
            }
        }

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_coolant_leak"));

            const auto ship = static_cast<Ship*>(Fluf::GetClient()->GetPlayerIObj());
            ship->light_fuse(0, CreateID("chaos_coolant_leak"), 0.0f, 5.0f, 0.0f);
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