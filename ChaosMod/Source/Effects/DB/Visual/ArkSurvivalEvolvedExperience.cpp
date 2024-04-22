#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/ReshadeManager.hpp"

class ArkSurvivalEvolvedExperience final : public ActiveEffect
{
        inline static std::array eyeBleeders = { CreateID("bw10"), CreateID("ew01"), CreateID("ew05"), CreateID("bw05") };
        inline static uint lastSystem = 0;

        void Begin() override
        {
            lastSystem = 0;
            const auto reshade = Get<ReshadeManager>();
            reshade->ToggleTechnique("qUINT_bloom.fx", "Bloom", true);
        }

        void Update(float delta) override
        {
            const auto ship = Utils::GetCShip();
            if (ship && ship->system != lastSystem)
            {
                const auto reshade = Get<ReshadeManager>();
                reshade->SetUniformFloat("qUINT_bloom.fx", "BLOOM_CURVE", { 1.5f, 0.f, 0.f }, 1);
                reshade->SetUniformFloat("qUINT_bloom.fx", "BLOOM_SAT", { 2.5f, 0.f, 0.f }, 1);

                if (std::ranges::find(eyeBleeders, ship->system) == eyeBleeders.end())
                {
                    reshade->SetUniformFloat("qUINT_bloom.fx", "BLOOM_INTENSITY", { 2.f, 0.f, 0.f }, 1);
                }
                else
                {
                    // Bad system, dial it down
                    reshade->SetUniformFloat("qUINT_bloom.fx", "BLOOM_INTENSITY", { 1.45f, 0.f, 0.f }, 1);
                }
                lastSystem == ship->system;
            }
        }


        void End() override { Get<ReshadeManager>()->ToggleTechnique("qUINT_bloom.fx", "Bloom", false); }

    public:
        explicit ArkSurvivalEvolvedExperience(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(ArkSurvivalEvolvedExperience, {
    .effectName = "Ark Survival Evolved Experience",
    .description = "The hit classic ARK is back to WOW you with next-gen graphics",
    .category = EffectType::Visual,
    .timingModifier = 1.f
});