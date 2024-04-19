// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField

#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class RadarJamming final : public ActiveEffect
{
        using GetTargetLeadFirePosType = bool(__fastcall*)(CShip* ship, void*, IObjInspect* target, Vector* output);
        inline static FunctionDetour<GetTargetLeadFirePosType> getTargetLeadFirePosDetour{ reinterpret_cast<GetTargetLeadFirePosType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?get_tgt_lead_fire_pos@CShip@@QBE_NPBUIObjInspect@@AAVVector@@@Z")) };

        using GetTargetLeadFirePosInspectType = int(__fastcall*)(IObjInspectImpl* inspect, void*, const uint16_t* id, Vector* output);
        inline static FunctionDetour<GetTargetLeadFirePosInspectType> getTargetLeadFirePosInspectDetour{ reinterpret_cast<GetTargetLeadFirePosInspectType>(
            GetProcAddress(GetModuleHandleA("common.dll"), "?get_tgt_lead_fire_pos@IObjInspectImpl@@UBEHABGAAVVector@@@Z")) };

        inline static constexpr auto fixedTime = 1 / 60.f;
        inline static Vector disabledVector = glm::vec3{ 99999.f, 99999.f, 99999.f };

        struct TargetData
        {
                int index = 0;
                float range = 0.0f;
                bool invert[3];
        };

        inline static std::unordered_map<void*, TargetData> jitterReducer;

        static void ApplyJitter(void* obj, Vector* output)
        {
            if (jitterReducer.contains(obj))
            {
                auto& data = jitterReducer[obj];
                if (data.index == 2)
                {
                    data.range = Get<Random>()->UniformFloat(-30.f, 30.f);
                    data.invert[0] = Get<Random>()->Uniform(0u, 1u) == 0u;
                    data.invert[1] = Get<Random>()->Uniform(0u, 1u) == 0u;
                    data.invert[2] = Get<Random>()->Uniform(0u, 1u) == 0u;
                }

                if (data.index++ >= 2)
                {
                    if (data.index > 6)
                    {
                        data.index = 0;
                    }

                    output->x += data.invert[0] ? -data.range : data.range;
                    output->y += data.invert[1] ? -data.range : data.range;
                    output->z += data.invert[2] ? -data.range : data.range;
                }
                else
                {
                    *output = disabledVector;
                }
            }
            else
            {
                jitterReducer[obj].index = 0;
            }
        }

        static bool __fastcall GetTargetLeadFirePosDetour(CShip* ship, void* edx, IObjInspect* target, Vector* output)
        {
            getTargetLeadFirePosDetour.UnDetour();
            const auto response = getTargetLeadFirePosDetour.GetOriginalFunc()(ship, edx, target, output);
            getTargetLeadFirePosDetour.Detour(GetTargetLeadFirePosDetour);

            if (response && ship == Utils::GetCShip())
            {
                ApplyJitter(ship, output);
            }

            return response;
        }

        static int __fastcall GetTargetLeadFirePosInspectDetour(IObjInspectImpl* inspect, void* edx, const uint16_t* id, Vector* output)
        {
            getTargetLeadFirePosInspectDetour.UnDetour();
            const auto response = getTargetLeadFirePosInspectDetour.GetOriginalFunc()(inspect, edx, id, output);
            getTargetLeadFirePosInspectDetour.Detour(GetTargetLeadFirePosInspectDetour);

            if (inspect->cobj != Utils::GetCShip() && response)
            {
                ApplyJitter(inspect, output);
            }

            return response;
        }

        void Begin() override
        {
            Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "GlitchB", true);
            jitterReducer.clear();

            getTargetLeadFirePosDetour.Detour(GetTargetLeadFirePosDetour);
            getTargetLeadFirePosInspectDetour.Detour(GetTargetLeadFirePosInspectDetour);
        }

        void End() override
        {
            Get<ReshadeManager>()->ToggleTechnique("ChaosMod.fx", "GlitchB", false);

            getTargetLeadFirePosDetour.UnDetour();
            getTargetLeadFirePosInspectDetour.UnDetour();
        }

    public:
        explicit RadarJamming(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(RadarJamming, {
    .effectName = "Radar Jamming",
    .description = "Being hacked makes aiming hard and seeing harder.",
    .category = EffectType::Interface
});