#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/KeyManager.hpp"

class IAmFiringMyLasers final : public ActiveEffect
{
        const std::array<Utils::Keys, 16> bannedKeys = { Utils::USER_FIRE_WEAPON1,
                                                         Utils::USER_FIRE_WEAPON2,           Utils::USER_FIRE_WEAPON3,
                                                         Utils::USER_FIRE_WEAPON4,           Utils::USER_FIRE_WEAPON5,
                                                         Utils::USER_FIRE_WEAPON6,           Utils::USER_FIRE_WEAPON7,
                                                         Utils::USER_FIRE_WEAPON8,           Utils::USER_FIRE_WEAPON9,
                                                         Utils::USER_FIRE_WEAPON10,          Utils::USER_FIRE_FORWARD,
                                                         Utils::USER_LAUNCH_COUNTERMEASURES, Utils::USER_LAUNCH_CRUISE_DISRUPTORS,
                                                         Utils::USER_LAUNCH_MINES,           Utils::USER_LAUNCH_MISSILES,
                                                         Utils::USER_LAUNCH_TORPEDOS };
        void Begin() override
        {
            for (const auto key : bannedKeys)
            {
                Get<KeyManager>()->ToggleAllowedKey(key, false);
            }
        }

        void End() override
        {
            for (const auto key : bannedKeys)
            {
                Get<KeyManager>()->ToggleAllowedKey(key, true);
            }
        }

        void Update(float delta) override
        {
            CShip* ship = Utils::GetCShip();
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