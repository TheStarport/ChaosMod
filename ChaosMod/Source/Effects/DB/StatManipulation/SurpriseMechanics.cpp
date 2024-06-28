#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class SurpriseMechanics final : public ActiveEffect
{
        std::vector<uint> chaosEquipment;

        void Begin() override
        {
            pub::Audio::PlaySoundEffect(1, CreateID("chaos_quite_ethical"));
            auto rand = Get<Random>();

            auto itemCount = rand->Uniform(0u, 4u);
            for (auto i = 0u; i <= itemCount; i++)
            {
                pub::Player::AddCargo(1, chaosEquipment[rand->Uniform(0u, chaosEquipment.size() - 1)], 1, 1.0f, false);
            }
        }

    public:
        explicit SurpriseMechanics(const EffectInfo& effectInfo) : ActiveEffect(effectInfo)
        {
            INI_Reader ini;
            ini.open("../DATA/CHAOS/chaos_player_equipment.ini", false);

            while (ini.read_header())
            {
                if (!ini.is_header("Gun") && !ini.is_header("Thruster") && !ini.is_header("ShieldGenerator"))
                {
                    continue;
                }

                while (ini.read_value())
                {
                    if (ini.is_value("nickname"))
                    {
                        chaosEquipment.emplace_back(CreateID(ini.get_value_string()));
                    }
                }
            }
        }
};

// clang-format off
SetupEffect(SurpriseMechanics, {
    .effectName = "Surprise Mechanics",
    .description = "We do think the way we've implemented these kinds of mechanics is quite ethical and quite fun. They aren't gambling and we disagree that there's evidence that shows they lead to gambling",
    .category = EffectType::StatManipulation,
    .timingModifier = 0.0f,
    .isTimed = false
});