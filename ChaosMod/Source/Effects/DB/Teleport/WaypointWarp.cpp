#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/Teleporter.hpp"

class WaypointWarp final : public ActiveEffect
{
        void Begin() override { Get<Teleporter>()->WaypointWarp(); }

    public:
        explicit WaypointWarp(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(WaypointWarp, {
    .effectName = "Waypoint Warp",
    .description = "When you really need to get where you're going.",
    .category = EffectType::Teleport,
    .timingModifier = 0.0f,
    .isTimed = false
});
