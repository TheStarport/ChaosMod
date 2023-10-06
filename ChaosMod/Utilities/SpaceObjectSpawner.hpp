#pragma once

#include <ResourcePtr.hpp>

struct SpawnedObject
{
        CSimple* obj;
        uint spaceObj;
};

class SpaceObjectSpawner final : public Singleton<SpaceObjectSpawner>
{
        struct NpcTemplate
        {
                std::string nickname;
                uint hash;
                std::string loadout;
                uint loadoutHash;
                std::string archetype;
                uint archetypeHash;
                std::string pilot;
                uint pilotHash;
                uint level;
        };

        std::unordered_map<uint, NpcTemplate> npcTemplates;
        std::vector<std::shared_ptr<SpawnedObject>> spawnedObjects;

    public:
        SpaceObjectSpawner();
        ~SpaceObjectSpawner();
        struct SpaceObjectBuilder
        {
                SpaceObjectBuilder& WithNpc(const std::string& npcNickname);
                SpaceObjectBuilder& WithArchetype(const std::string& archetype);
                SpaceObjectBuilder& WithArchetype(uint archetype);
                SpaceObjectBuilder& WithLoadout(const std::string& loadout);
                SpaceObjectBuilder& WithLoadout(uint loadout);
                SpaceObjectBuilder& WithCostume(const Costume& costume);
                SpaceObjectBuilder& WithPersonality(const std::string& personalityNickname);
                SpaceObjectBuilder& WithPersonality(pub::AI::Personality& personality);
                SpaceObjectBuilder& WithPosition(const Vector& positionVector, float variance = 0.0f);
                SpaceObjectBuilder& WithRotation(const Vector& euler);
                SpaceObjectBuilder& WithRotation(const Matrix& orientation);
                SpaceObjectBuilder& WithSystem(uint systemHash);
                SpaceObjectBuilder& WithSystem(const std::string& systemNick);
                SpaceObjectBuilder& WithAbsoluteHealth(float absoluteHealth);
                SpaceObjectBuilder& WithRelativeHealth(float relativeHealth);
                SpaceObjectBuilder& WithLevel(uint level);
                SpaceObjectBuilder& WithVoice(const std::string& voice);
                SpaceObjectBuilder& WithName(uint firstName, uint secondName = 0);
                SpaceObjectBuilder& WithName(FmtStr& scannerName, FmtStr& pilotName);
                SpaceObjectBuilder& WithReputation(const std::string& rep);
                SpaceObjectBuilder& WithReputation(uint affiliation);
                SpaceObjectBuilder& WithDockTo(uint base);

                SpaceObjectBuilder& AsSolar();
                SpaceObjectBuilder& AsNpc();
                ResourcePtr<SpawnedObject> Spawn();

            private:
                NpcTemplate npcTemplate;
                std::optional<Costume> costumeOverride;
                std::optional<uint> voiceOverride;
                std::optional<const pub::AI::Personality*> personalityOverride;
                std::optional<Matrix> rotation;
                std::optional<Vector> position;
                std::optional<float> positionVariance;
                std::optional<uint> system;
                std::optional<float> health;
                std::optional<std::string> reputation;
                std::optional<uint> affiliation;
                std::optional<std::pair<uint, uint>> name;
                std::optional<uint> dockTo;
                std::optional<std::pair<FmtStr&, FmtStr&>> names;

                std::array<uint, 21> defaultNames = { 197808u, 197809u, 197810u, 197811u, 197812u, 197813u, 197814u, 197815u, 197816u, 197817u, 197818u,
                                                      197819u, 197820u, 197821u, 197822u, 197823u, 197824u, 197825u, 197826u, 197827u, 197828 };

                bool isNpc = true;
                bool healthRelative = true;

                friend SpaceObjectSpawner;
                std::weak_ptr<SpawnedObject> SpawnNpc();
                std::weak_ptr<SpawnedObject> SpawnSolar();
                void ValidateSpawn();
                SpaceObjectBuilder() = default;
        };

        static SpaceObjectBuilder NewBuilder() { return {}; }

        void Destroy(ResourcePtr<SpawnedObject> object);
        void Despawn(ResourcePtr<SpawnedObject> object);
};
