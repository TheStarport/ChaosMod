#pragma once
#include "Effects/ActiveEffect.hpp"

class Pacifist final : public ActiveEffect
{
        float timer = 0.0f;
        bool shouldDie = false;

        void Begin() override { shouldDie = false; }

        void OnShipDestroyed(DamageList* dmgList, CShip* ship) override
        {
            if (const auto player = Utils::GetCShip(); !shouldDie && player && player != ship && dmgList->inflictorPlayerId == 1)
            {
                //const FmtStr fmt(002153, nullptr);
                const FmtStr fmt(458759, nullptr);
                pub::Player::DisplayMissionMessage(1, fmt, MissionMessageType::Type2, false);

                timer = 3.0f;
                shouldDie = true;

                ChaosTimer::i()->DelayActiveEffect(this, 3.0f);
            }
        }

        void Update(const float delta) override
        {
            if (shouldDie)
            {
                const auto player = Utils::GetCShip();
                timer -= delta;
                if (timer <= 0.0f && player)
                {
                    // Kill the player. AN EYE FOR AN EYE
                    pub::SpaceObj::Destroy(player->id, DestroyType::Fuse);
                }
            }
        }

    public:
        DefaultEffectInfo("Pacifist", EffectType::Npc);
};
