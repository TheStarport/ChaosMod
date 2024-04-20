#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/Teleporter.hpp"

class Overslept final : public ActiveEffect
{
        // To Omicron Alpha you go!
        void Begin() override { Get<Teleporter>()->WarpToPoint(CreateID("hi01"), { -7480.f, 67.f, 26175.f }, { -30.f, 8.f, 0.f }); }

    public:
        explicit Overslept(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Overslept, {
    .effectName = "Overslept",
    .description = "Ahh, you're finally awake! You were trying to flee the coalition right? Same us as, and the Corsair over there.",
    .category = EffectType::Teleport,
    .timingModifier = 0.0f,
    .isTimed = false
});
