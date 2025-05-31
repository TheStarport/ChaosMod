#include "Components/ShipManipulator.hpp"
#include "PCH.hpp"

#include "Effects/PersistentEffect.hpp"

class RunningOnFumes final : public PersistentEffect
{
        inline static float fuelPercentage = 100.f;

        using EngineDragDetourType = float(__fastcall*)(CEEngine*);
        using DragDetourType = float(__fastcall*)(CShip*);
        inline static FunctionDetour<EngineDragDetourType> linearDragDetour{ reinterpret_cast<EngineDragDetourType>(
            reinterpret_cast<DWORD>(GetModuleHandleA("common.dll")) + 0x361E0) };
        inline static FunctionDetour<DragDetourType> linearCruiseDragDetour{ reinterpret_cast<DragDetourType>(
            reinterpret_cast<DWORD>(GetModuleHandleA("common.dll")) + 0x53730) };

        static float __fastcall LinearDragDetour(CEEngine* engine)
        {
            linearDragDetour.UnDetour();
            float drag = linearDragDetour.GetOriginalFunc()(engine);
            linearDragDetour.Detour(LinearDragDetour);

            if (fuelPercentage < 0.0f && engine->owner == Utils::GetCShip())
            {
                drag *= 2.0f;
            }

            return drag;
        }

        static float __fastcall LinearCruiseDragDetour(CShip* ship)
        {
            linearCruiseDragDetour.UnDetour();
            float drag = linearCruiseDragDetour.GetOriginalFunc()(ship);
            linearCruiseDragDetour.Detour(LinearCruiseDragDetour);

            if (fuelPercentage < 0.0f && ship == Utils::GetCShip())
            {
                drag *= .4f;
            }

            return drag;
        }

        void Begin() override
        {
            PersistentEffect::Begin();

            linearDragDetour.Detour(LinearDragDetour);
            linearCruiseDragDetour.Detour(LinearCruiseDragDetour);

            pub::Player::AddCargo(1, CreateID("commodity_h_fuel"), 5, 1.0f, false);

            FmtStr caption(0, nullptr);
            caption.begin_mad_lib(458877);
            caption.end_mad_lib();

            FmtStr message(0, nullptr);
            message.begin_mad_lib(458878);
            message.end_mad_lib();

            pub::Player::PopUpDialog(1, caption, message, static_cast<int>(PopupDialog::CenterOk));
        }

        void End() override
        {
            PersistentEffect::End();

            linearDragDetour.UnDetour();
            linearCruiseDragDetour.UnDetour();
        }

        std::array<uint, 2> possibleFuel = { CreateID("commodity_mox_fuel"), CreateID("commodity_H_fuel") };

        float timeSinceFuel = 1.f;
        void Update(const float delta) override
        {
            Log(std::to_string(fuelPercentage));
            auto* ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            timeSinceFuel -= delta;
            if (timeSinceFuel < 0.0f)
            {
                timeSinceFuel = 1.0f;

                const float totalSpeed = std::abs(glm::length<3, float, glm::packed_highp>(ship->get_velocity()));
                const float consumption = totalSpeed / 300.f * 3.3f;
                fuelPercentage -= consumption;

                if (fuelPercentage > 0.f)
                {
                    return;
                }

                ushort fuelToRemove = 0;
                for (auto traverser = ship->equipManager.begin(); traverser != ship->equipManager.end(); ++traverser)
                {
                    if (std::ranges::find(possibleFuel, traverser.currentEquip->archetype->archId) != possibleFuel.end())
                    {
                        EquipDesc desc;
                        traverser.currentEquip->GetEquipDesc(desc);

                        fuelToRemove = desc.id;
                        break;
                    }
                }

                if (fuelToRemove)
                {
                    pub::Player::RemoveCargo(1, fuelToRemove, 1);
                    fuelPercentage = 100.f;
                }
                else
                {
                    fuelPercentage = -1.0f;
                }
            }
        }

    public:
        explicit RunningOnFumes(const EffectInfo& effectInfo) : PersistentEffect(effectInfo) {}
};

// clang-format off
SetupEffect(RunningOnFumes, {
    .effectName = "Running On Fumes",
    .description = "Your ship now consumes fuel, without fuel how will you go SPEEDY ZOOMY?",
    .category = EffectType::Misc,
    .timingModifier = 0.0f,
    .isTimed = false
});