#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/Teleporter.hpp"

class FakeTeleport final : public ActiveEffect
{
        void Begin() override
        {
            Teleporter::i()->SaveCurrentPosition();
            Teleporter::i()->WarpToRandomSolar(false);
        }

        void End() override { Teleporter::i()->RestorePreviousPosition(); }

    public:
        explicit FakeTeleport(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(FakeTeleport, {
    .effectName = "Fake Teleport",
    .description = "Like a real teleport, but we put you back afterwards. Might still break your missions though, please don't hate me.",
    .category = EffectType::Teleport,
    .fixedTimeOverride = 5.0f
});