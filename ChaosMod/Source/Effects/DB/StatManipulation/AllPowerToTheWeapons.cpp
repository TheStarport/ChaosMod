#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class AllPowerToTheWeapons final : public ActiveEffect
{
        using RefireDelayElapsedType = bool(__fastcall*)(CELauncher*);

        // Copied from IDA with a minor change (double refire BABY)
        static bool __fastcall RefireDelayElapsed(CELauncher* launcher)
        {
            return *(reinterpret_cast<float*>(launcher) + 20) >=
                   static_cast<double>(*reinterpret_cast<float*>(*(reinterpret_cast<DWORD*>(launcher) + 3) + 144)) * 0.5f;
        }

        inline static FunctionDetour<RefireDelayElapsedType> refireDelayElapsedDetour{ reinterpret_cast<RefireDelayElapsedType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?RefireDelayElapsed@CELauncher@@MBE_NXZ")) };

        void Begin() override { refireDelayElapsedDetour.Detour(RefireDelayElapsed); }

        void End() override { refireDelayElapsedDetour.UnDetour(); }

        void Update(float delta) override
        {
            auto ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            float capacity = 0.f;

            auto traverser = ship->equip_manager.StartTraverse(EquipmentClass::Power);
            auto equip = ship->equip_manager.Traverse(traverser);
            while (equip)
            {
                capacity += ((CEPower*)equip)->GetCapacity();
                equip = ship->equip_manager.Traverse(traverser);
            }

            // Infinite power while enabled
            ship->power = capacity;

            // Remove the shields from the player ship
            pub::SpaceObj::DrainShields(ship->id);
        }

    public:
        explicit AllPowerToTheWeapons(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(AllPowerToTheWeapons, {
    .effectName = "All Power To The Weapons",
    .description = "With all the power diverted to the weapons, you may find trouble maintaining your shield.",
    .category = EffectType::StatManipulation,
    .exclusion = EffectExclusion::RefireDelay
});