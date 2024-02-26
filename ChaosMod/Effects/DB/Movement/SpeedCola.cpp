#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

#include <magic_enum_flags.hpp>

class SpeedCola final : public ActiveEffect
{
        using GetMassType = float(__fastcall*)(CShip* obj);
        inline static FunctionDetour<GetMassType> getMassDetour{ reinterpret_cast<GetMassType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?get_mass@CObject@@UBEMXZ")) };

        static float __fastcall GetMassDetour(CShip* obj)
        {
            getMassDetour.UnDetour();
            const float orig = getMassDetour.GetOriginalFunc()(obj);
            getMassDetour.Detour(GetMassDetour);

            if (Utils::GetCShip() != obj || !magic_enum::enum_flags_test(obj->objectClass, CObject::CSHIP_OBJECT))
            {
                return orig;
            }

            return orig * 3.0f;
        }

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_speed_cola"));
            getMassDetour.Detour(GetMassDetour);
        }
        void End() override { getMassDetour.UnDetour(); }

    public:
        explicit SpeedCola(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SpeedCola, {
    .effectName = "Speed Cola",
    .description = "Speed Cola speeds up your life.",
    .category = EffectType::Movement,
    .timingModifier = 1.5f,
    .exclusion = EffectExclusion::GetMass
});