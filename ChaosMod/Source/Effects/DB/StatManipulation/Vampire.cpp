#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Vampire final : public ActiveEffect
{
        float damageTimer = 0.0f;
        void OnShipDestroyed(DamageList* dmgList, CShip* ship) override
        {
            if (dmgList->inflictorPlayerId != 1)
            {
                return;
            }

            const auto player = Utils::GetCShip();

            float relativeHealth = 0.f;
            pub::SpaceObj::GetRelativeHealth(player->id, relativeHealth);
            pub::SpaceObj::SetRelativeHealth(player->id, std::clamp(relativeHealth + 0.50f, 0.f, 1.f)); // Add 50% of their health back
        }

        void Begin() override { damageTimer = 1.0f; }

        void Update(float delta) override
        {
            auto ship = Utils::GetCShip();
            damageTimer -= delta;
            if (!ship || damageTimer > 0.f)
            {
                return;
            }

            damageTimer = 1.f;

            // The player doesn't die when their health is set to 0%, so they get another 1 second to kill something.
            if (ship->get_relative_health() < 0.03f)
            {
                pub::SpaceObj::Destroy(ship->id, DestroyType::Fuse);
                return;
            }

            bool inCombat = false;
            Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT,
                                        [ship, &inCombat](auto npc)
                                        {
                                            if (ship == npc || inCombat || glm::distance<3, float, glm::packed_highp>(ship->position, npc->position) > 5000.f)
                                            {
                                                return;
                                            }

                                            int reputation;
                                            pub::Player::GetRep(1, reputation);
                                            int npcReputation;
                                            pub::SpaceObj::GetRep(npc->id, npcReputation);

                                            float rep;
                                            pub::Reputation::GetAttitude(npcReputation, reputation, rep);

                                            if (rep < -0.59999f)
                                            {
                                                inCombat = true;
                                            }
                                        });

            if (!inCombat)
            {
                return;
            }

            float relativeHealth = 0.f;
            pub::SpaceObj::GetRelativeHealth(ship->id, relativeHealth);
            pub::SpaceObj::SetRelativeHealth(ship->id, relativeHealth - 0.03f); // Remove 3% of their health
        }

    public:
        explicit Vampire(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Vampire, {
    .effectName = "Vampire",
    .description = "Like a vampire, you must kill in order to sustain yourself. Don't worry, the damage is disabled out of combat.",
    .category = EffectType::StatManipulation,
});