#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ShipManipulator.hpp"

class Stalled final : public ActiveEffect
{
        using CEActivateType = bool(__fastcall*)(void* engine, void* edx, bool state);
        inline static FunctionDetour<CEActivateType> ceEngineActivateDetour{ reinterpret_cast<CEActivateType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?Activate@CEEngine@@UAE_N_N@Z")) };
        inline static FunctionDetour<CEActivateType> ceThrusterActivateDetour{ reinterpret_cast<CEActivateType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?Activate@CEThruster@@UAE_N_N@Z")) };

        static bool __fastcall EngineActiveDetour(void* engine, void* edx, bool state) { return false; }

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_stalled"));

            auto ship = Utils::GetCShip();
            CEquipTraverser traverser{ static_cast<int>(EquipmentClass::Engine) };
            CEquip* equip;

            while ((equip = ship->equip_manager.Traverse(traverser)))
            {
                equip->Activate(false);
            }

            traverser = { static_cast<int>(EquipmentClass::Thruster) };

            while ((equip = ship->equip_manager.Traverse(traverser)))
            {
                equip->Activate(false);
            }

            ceEngineActivateDetour.Detour(EngineActiveDetour);
            ceThrusterActivateDetour.Detour(EngineActiveDetour);
        }

        void End() override
        {
            ceEngineActivateDetour.UnDetour();
            ceThrusterActivateDetour.UnDetour();
        }

    public:
        explicit Stalled(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Stalled, {
    .effectName = "Stalled",
    .description = "Your ship's engine appears to have broken, better see a mechanic!",
    .category = EffectType::Movement,
    .fixedTimeOverride = 24.f
});
