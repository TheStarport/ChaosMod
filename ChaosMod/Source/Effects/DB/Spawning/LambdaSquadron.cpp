#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Components/SpaceObjectSpawner.hpp"

class LambdaSquadron final : public ActiveEffect
{
        std::vector<ResourcePtr<SpawnedObject>> npcs;
        void Spawn()
        {
            const auto ship = Utils::GetCShip();

            for (auto npc : npcs)
            {
                if (npc.Acquire())
                {
                    Get<SpaceObjectSpawner>()->Despawn(npc);
                }
            }
            npcs.clear();

            auto npc = SpaceObjectSpawner::NewBuilder()
                           .WithNpc("MSN11_Walker")
                           .WithSystem(ship->system)
                           .WithPosition(ship->get_position(), 500.f)
                           .WithLevel(40)
                           .WithReputation("li_n_grp")
                           .WithName(216302)
                           .WithFuse("chaos_teleport_fx")
                           .Spawn();

            SaveNpc(ship, npc);

            for (auto i = 216404u; i < 216409u; i++)
            {
                npc = SpaceObjectSpawner::NewBuilder()
                          .WithNpc("MSN11_Nomad_Liberty_Heavy_Fighter")
                          .WithSystem(ship->system)
                          .WithPosition(ship->get_position(), 250.f)
                          .WithLevel(40)
                          .WithReputation("li_n_grp")
                          .WithName(i)
                          .WithFuse("chaos_teleport_fx")
                          .Spawn();

                SaveNpc(ship, npc);
            }

            npc = SpaceObjectSpawner::NewBuilder()
                      .WithNpc("MSN11_King")
                      .WithSystem(ship->system)
                      .WithPosition(ship->get_position(), 300.f)
                      .WithLevel(40)
                      .WithReputation("li_lsf_grp")
                      .WithName(216306)
                      .WithFuse("chaos_teleport_fx")
                      .Spawn();

            SaveNpc(ship, npc);
        }

        void SaveNpc(CShip* ship, ResourcePtr<SpawnedObject> npc)
        {
            if (npc.Acquire())
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

        void Begin() override { Spawn(); }
        void OnLoad() override { Spawn(); }

    public:
        explicit LambdaSquadron(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(LambdaSquadron, {
    .effectName = "Lambda Squadron",
    .description = "Lambda Squadron is on their way to save the day",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});