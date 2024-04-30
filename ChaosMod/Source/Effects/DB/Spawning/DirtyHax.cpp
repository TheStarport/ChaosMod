#include "PCH.hpp"

#include "Components/SpaceObjectSpawner.hpp"
#include "Effects/ActiveEffect.hpp"
#include "Effects/MemoryEffect.hpp"

class DirtyHax final : public MemoryEffect
{
    using MemoryEffect::MemoryEffect;

        // clang-format off
        MemoryListStart(offsets)
        MemoryListItem("common.dll", ShipCanFireWhenCloaked, sizeof(float))
        MemoryListEnd(offsets);
        // clang-format on

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

            for (auto i = 0; i < 5; i++)
            {
                const auto npc = SpaceObjectSpawner::NewBuilder()
                          .WithNpc("rh_n_rh_elite_d12-19")
                          .WithSystem(ship->system)
                          .WithPosition(ship->get_position(), 250.f)
                          .WithLevel(40)
                          .WithReputation("fc_rn_grp")
                          .WithName(216403)
                          .WithFuse("chaos_teleport_fx")
                          .Spawn();

                SaveNpc(ship, npc);
            }
        }

        void SaveNpc(const CShip* ship, ResourcePtr<SpawnedObject> npc)
        {
            if (auto acquired = npc.Acquire())
            {
                dynamic_cast<CShip*>(acquired->obj)->cloakPercentage = 1.f;
                npcs.emplace_back(npc);

                pub::AI::DirectiveTrailOp op;
                op.x0C = ship->id;
                op.x10 = 500.0f;
                op.x14 = true;
                op.fireWeapons = true;
                pub::AI::SubmitDirective(npc.Acquire()->spaceObj, &op);
            }
        }

        void Begin() override
        {
            MemoryEffect::Begin();

            constexpr std::array<byte, 1> newFireResult = { 0x09 };
            MemUtils::WriteProcMem(offsets[0].module + offsets[0].offset, newFireResult.data(), newFireResult.size());

            Spawn();
        }

        void OnLoad() override { Spawn(); }
    
        void End() override
        {
            MemoryEffect::End();

            for (auto npc : npcs)
            {
                if (const auto resource = npc.Acquire())
                {
                    dynamic_cast<CShip*>(resource->obj)->cloakPercentage = 0.f;
                }
            }
        }
};

// clang-format off
SetupEffect(DirtyHax, {
    .effectName = "Dirty Hax",
    .description = "Those dirty cheating NPCs, how is this even fair?",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});