#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ShipManipulator.hpp"

class PersonalSpace final : public ActiveEffect
{
        void Begin() override { Get<ShipManipulator>()->SetPersonalSpace(true); }
        void End() override { Get<ShipManipulator>()->SetPersonalSpace(false); }

    public:
        explicit PersonalSpace(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(PersonalSpace, {
    .effectName = "Personal Space",
    .description = "These rude NPCs, getting all in your face... so much easier to shove them away with a giant forcefield!",
    .category = EffectType::Movement
});
