#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include <magic_enum_flags.hpp>

// This effect is 'always active' in that the NPCs are always slightly faster,
// but when the effect activates they go even faster.

class SwiftNPCs final : public ActiveEffect
{
        using GetMass = float(__fastcall*)(CObject* obj);
        inline static const std::unique_ptr<FunctionDetour<GetMass>> detour =
            std::make_unique<FunctionDetour<GetMass>>(reinterpret_cast<GetMass>(GetProcAddress(GetModuleHandleA("common.dll"), "?get_mass@CObject@@UBEMXZ")));

        inline static float modifier = 1.5f;

        static float __fastcall Detour(CObject* obj)
        {
            detour->UnDetour();
            const float orig = detour->GetOriginalFunc()(obj);
            detour->Detour(Detour);

            if (Utils::GetCShip() == obj || !magic_enum::enum_flags_test(obj->objectClass, CObject::CSHIP_OBJECT))
            {
                return orig;
            }

            // For reasons I am very unsure of, increasing the mass of NPCs makes them go faster??
            // dunno why, but giving them 3x mass seems to make them very zippy.
            return orig * modifier;
        }

        void Init() override { detour->Detour(Detour); }
        void Begin() override { modifier = 1.5f; }
        void End() override { modifier = 3.5f; }

    public:
        explicit SwiftNPCs(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(SwiftNPCs, {
    .effectName = "Swift NPCs",
    .description = "Someone made the NPCs go all zoomy!",
    .category = EffectType::Npc,
    .exclusion = EffectExclusion::GetMass
});