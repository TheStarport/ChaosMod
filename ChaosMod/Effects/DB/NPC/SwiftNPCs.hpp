#pragma once
#include "Effects/ActiveEffect.hpp"

#include <magic_enum_flags.hpp>

class SwiftNPCs final : public ActiveEffect
{
        using GetMass = float(__fastcall*)(CObject* obj);
        inline static const std::unique_ptr<FunctionDetour<GetMass>> detour =
            std::make_unique<FunctionDetour<GetMass>>(reinterpret_cast<GetMass>(GetProcAddress(GetModuleHandleA("common.dll"), "?get_mass@CObject@@UBEMXZ")));

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
            return orig * 3.0f;
        }

        void Begin() override { detour->Detour(Detour); }
        void End() override { detour->UnDetour(); }

    public:
        DefaultEffectInfo("Swift NPCs", EffectType::Npc);
};
