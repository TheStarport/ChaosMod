#pragma once
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
        DefaultEffectInfo("Laggy Player", EffectType::Misc);
};
