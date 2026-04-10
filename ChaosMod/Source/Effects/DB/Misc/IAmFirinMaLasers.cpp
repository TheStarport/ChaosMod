#include "Effects/ActiveEffect.hpp"
class IAmFiringMyLasers final : public ActiveEffect
{
        const std::array<Key, 16> bannedKeys = { Key::USER_FIRE_WEAPON1,
                                                         Key::USER_FIRE_WEAPON2,           Key::USER_FIRE_WEAPON3,
                                                         Key::USER_FIRE_WEAPON4,           Key::USER_FIRE_WEAPON5,
                                                         Key::USER_FIRE_WEAPON6,           Key::USER_FIRE_WEAPON7,
                                                         Key::USER_FIRE_WEAPON8,           Key::USER_FIRE_WEAPON9,
                                                         Key::USER_FIRE_WEAPON10,          Key::USER_FIRE_FORWARD,
                                                         Key::USER_LAUNCH_COUNTERMEASURES, Key::USER_LAUNCH_CRUISE_DISRUPTORS,
                                                         Key::USER_LAUNCH_MINES,           Key::USER_LAUNCH_MISSILES,
                                                         Key::USER_LAUNCH_TORPEDOS };
        void Begin() override
        {
            for (const auto key : bannedKeys)
            {
                // TODO: Get<KeyManager>()->ToggleAllowedKey(key, false);
            }
        }

        void End() override
        {
            for (const auto key : bannedKeys)
            {
                // TODO: Get<KeyManager>()->ToggleAllowedKey(key, true);
            }
        }

        void Update(float delta) override
        {
            CShip* ship = Fluf::GetClient()->GetPlayerCShip();
            if (!ship)
            {
                return;
            }

            INPUT inputs[1];

            inputs[0].type = INPUT_MOUSE;
            inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

            SendInput(1, inputs, sizeof(INPUT));
        }

    public:
        explicit IAmFiringMyLasers(const EffectInfo& info) : ActiveEffect(info) {}
};

// clang-format off
SetupEffect(IAmFiringMyLasers, {
    .effectName = "I'm Firin Ma Lasers",
    .description = "ALL YOUR GUNS GO BRRRRRRR!",
    .category = EffectType::Misc
});