#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class LaggyPlayer final : public ActiveEffect
{
        float interval = 1.5f;
        uint state = 0;

        Vector pos{};
        Matrix rotation{};

        void Begin() override
        {
            state = 0;
            interval = 1.5f;
        }

        void Update(const float delta) override
        {
            interval -= delta;
            if (interval > 0.0f)
            {
                return;
            }

            interval += 1.5f;

            if (++state == 4)
            {
                state = 0;
            }

            const auto player = Utils::GetCShip();
            if (state == 2)
            {
                pos = player->get_position();
                rotation = player->get_orientation();
            }
            else if (state == 3)
            {
                player->beam_object(pos, rotation, true);
            }
        }

    public:
        explicit LaggyPlayer(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(LaggyPlayer, {
    .effectName = "Laggy Player",
    .description = "\"I swear I died to lag!\"\n\"...Isn't this a singleplayer game?\"\n\"Trust me is was lag my dude!!!!\"",
    .category = EffectType::Misc
});