#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Yeet final : public ActiveEffect
{
        Vector movement{};
        void Begin() override
        {
            auto ship = Utils::GetCShip();

            ship->get_behavior_interface()->update_current_behavior_engage_engine(false);

            movement.x = Random::i()->Uniform(0, 1) ? -100000.f : 100000.f;
            movement.y = Random::i()->Uniform(0, 1) ? -100000.f : 100000.f;
            movement.z = Random::i()->Uniform(0, 1) ? -100000.f : 100000.f;

            const uint ptr = *reinterpret_cast<uint*>(PCHAR(*reinterpret_cast<uint*>(uint(ship) + 84)) + 152);
            *reinterpret_cast<Vector*>(ptr + 164) = movement;
        }

        // Ensure they cannot restart their engine for 5% of effect duration
        void Update(float delta) override
        {
            auto ship = Utils::GetCShip();
            ship->get_behavior_interface()->update_current_behavior_engage_engine(false);
            const uint ptr = *reinterpret_cast<uint*>(PCHAR(*reinterpret_cast<uint*>(uint(ship) + 84)) + 152);
            *reinterpret_cast<Vector*>(ptr + 164) = movement;
        }

    public:
        explicit Yeet(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Yeet, {
    .effectName = "YEET",
    .description = "God decided you are to be yeeted away at high speed.",
    .category = EffectType::Movement,
    .fixedTimeOverride = 4.0f
});