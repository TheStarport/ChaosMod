#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class BloodMagic final : public ActiveEffect
{
        using ConsumeFireResourcesType = void(__fastcall*)(CELauncher*);

        void OnConsumeFireResources(CELauncher* launcher) override
        {
            const auto ship = Utils::GetCShip();
            if (launcher->GetOwner() != ship)
            {
                return;
            }

            // Massively lower the health cost so the user doesn't instantly die
            const float healthCost = launcher->LauncherArch()->powerUsage * 0.2f;
            ship->hitPoints -= healthCost;

            // Prevent their health from going negative, kill them now
            if (ship->get_relative_health() < 0.001f)
            {
                pub::SpaceObj::Destroy(ship->id, DestroyType::Fuse);
                return;
            }
        }

        void Update(float delta) override
        {
            auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            float capacity = 0.f;

            auto traverser = ship->equipManager.StartTraverse(EquipmentClass::Power);
            auto equip = ship->equipManager.Traverse(traverser);
            while (equip)
            {
                capacity += static_cast<CEPower*>(equip)->GetCapacity();
                equip = ship->equipManager.Traverse(traverser);
            }

            // While blood magic mode is on, always be at 100% power.
            // HEALTH IS OUR CURRENCY FOR WAR
            ship->power = capacity;
        }

    public:
        explicit BloodMagic(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(BloodMagic, {
    .effectName = "Blood Magic",
    .description = "Blood magic is a school of magic that taps into the inherent power of blood to fuel spellcasting. "
        "It can do great evil at the expense of the caster's health, or the expense of others.",
    .category = EffectType::StatManipulation,
    .exclusion = EffectExclusion::FireResource
});