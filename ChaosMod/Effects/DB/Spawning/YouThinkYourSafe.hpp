#pragma once

#include "Effects/ActiveEffect.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class YouThinkYourSafe final : public ActiveEffect
{
        std::vector<ResourcePtr<SpawnedObject>> npcs;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            for (auto npc : npcs)
            {
                if (npc.Acquire())
                {
                    SpaceObjectSpawner::i()->Despawn(npc);
                }
            }
            npcs.clear();

            const uint count = Random::i()->Uniform(2u, 5u);
            for (auto i = 0u; i < count; i++)
            {
                if (auto npc = SpaceObjectSpawner::NewBuilder()
                                   .WithNpc("SETSCENE_M03_Navy_Fighter")
                                   .WithSystem(ship->system)
                                   .WithPosition(ship->get_position(), 250.f)
                                   .WithLevel(25)
                                   .WithReputation("li_lsf_grp")
                                   .WithName(458806)
                                   .Spawn();
                    npc.Acquire())
                {
                    npcs.emplace_back(npc);

                    pub::AI::DirectiveTrailOp op;
                    op.x0C = ship->id;
                    op.x10 = 1000.0f;
                    op.x14 = true;
                    op.fireWeapons = true;
                    pub::AI::SubmitDirective(npc.Acquire()->spaceObj, &op);
                }
            }

            pub::Audio::PlaySoundEffect(1, CreateID("chaos_you_think_your_safe"));
        }

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        DefEffectInfo("You Think Your Safe", 2.0f, EffectType::Spawning);
};
