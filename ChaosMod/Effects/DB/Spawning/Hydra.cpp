#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Utilities/AssetTracker.hpp"
#include "Utilities/SpaceObjectSpawner.hpp"

class Hydra final : public ActiveEffect
{
        void OnShipDestroyed(DamageList* dmgList, CShip* ship) override
        {
            if (dmgList->damageCause != DamageCause::Gun && dmgList->damageCause != DamageCause::Collision &&
                dmgList->damageCause != DamageCause::CruiseDisrupter && dmgList->damageCause != DamageCause::Mine &&
                dmgList->damageCause != DamageCause::MissileTorpedo)
            {
                return;
            }

            auto* arch = ship->shiparch();

            pub::AI::Personality personality;
            pub::AI::get_personality(ship->id, personality);

            int rep;
            pub::SpaceObj::GetRep(ship->id, rep);
            FmtStr name1(0, nullptr);
            FmtStr name2(0, nullptr);
            const ushort* str = nullptr;
            Reputation::Vibe::GetName(rep, name1, name2, str);

            const auto npc = AssetTracker::GetNpcCreationParams(ship->id);
            if (!npc)
            {
                return;
            }

            uint affiliation;
            pub::Reputation::GetAffiliation(rep, affiliation);

            auto builder = SpaceObjectSpawner::NewBuilder()
                               .WithArchetype(npc->arch)
                               .WithPosition(ship->position, 50.0f)
                               .WithSystem(ship->system)
                               .WithLoadout(npc->loadout)
                               .WithReputation(affiliation)
                               .WithPersonality(personality)
                               .WithName(name1, name2)
                               .WithLevel(npc->level);

            auto head1 = builder.Spawn().Acquire();
            auto head2 = builder.Spawn().Acquire();

            // TODO: set AI directives
        }

    public:
        explicit Hydra(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(Hydra, {
    .effectName = "Hydra",
    .description = "\"For every head you remove, two more shall take its place. You can strike me down, but my friends will avenge me.\"",
    .category = EffectType::Spawning,
    .timingModifier = 2.0f
});