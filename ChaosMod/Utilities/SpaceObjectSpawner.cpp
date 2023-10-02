#include "PCH.hpp"

#include "SpaceObjectSpawner.hpp"

#include "Exceptions/NpcLoadingException.hpp"
#include "PersonalityHelper.hpp"

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithNpc(const std::string& npcNickname)
{
    auto& templates = i()->npcTemplates;
    const auto found = templates.find(CreateID(npcNickname.c_str()));
    if (found == templates.end())
    {
        throw NpcLoadingException("NPC Nickname not found. This means either the provided name was invalid, or the NPC it represented was invalid.");
    }

    isNpc = true;
    npcTemplate = found->second;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithArchetype(const std::string& archetype)
{
    npcTemplate.archetype = archetype;
    npcTemplate.archetypeHash = CreateID(archetype.c_str());
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithLoadout(const std::string& loadout)
{
    if (!Loadout::Get(CreateID(loadout.c_str())))
    {
        throw NpcLoadingException(std::format("{} loadout was not found while loading NPC.", loadout));
    }

    npcTemplate.loadout = loadout;
    npcTemplate.loadoutHash = CreateID(loadout.c_str());
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithCostume(const Costume& costume)
{
    costumeOverride.emplace(costume);
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithPersonality(const std::string& personalityNickname)
{
    if (const auto personality = PersonalityHelper::i()->Get(personalityNickname); personality.has_value())
    {
        personalityOverride = personality.value();
    }

    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithPosition(const Vector& positionVector, float variance)
{
    position = positionVector;
    if (variance > 1.0f)
    {
        positionVariance = variance;
    }

    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithRotation(const Vector& euler)
{
    rotation = EulerMatrix(euler);
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithRotation(const Matrix& orientation)
{
    rotation = orientation;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithSystem(uint systemHash)
{
    system = systemHash;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithSystem(const std::string& systemNick)
{
    if (auto id = Universe::get_system_id(systemNick.c_str()))
    {
        system = id;
    }

    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithAbsoluteHealth(float absoluteHealth)
{
    healthRelative = false;
    health = absoluteHealth;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithRelativeHealth(float relativeHealth)
{
    healthRelative = true;
    health = relativeHealth;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithLevel(const uint level)
{
    npcTemplate.level = level;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithVoice(const std::string& voice)
{
    voiceOverride = CreateID(voice.c_str());
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithName(uint firstName, uint secondName)
{
    name = std::make_pair(firstName, secondName);
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithReputation(const std::string& rep)
{
    reputation = rep;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithDockTo(uint base)
{
    dockTo = base;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::AsSolar()
{
    isNpc = false;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::AsNpc()
{
    isNpc = true;
    return *this;
}

ResourcePtr<SpawnedObject> SpaceObjectSpawner::SpaceObjectBuilder::Spawn()
{
    ValidateSpawn();

    static const auto validateExisting = [](const std::shared_ptr<SpawnedObject>& obj) { return pub::SpaceObj::ExistsAndAlive(obj->spaceObj) != -2; };

    if (isNpc)
    {
        return { SpawnNpc(), validateExisting };
    }

    return { SpawnSolar(), validateExisting };
}

std::weak_ptr<SpawnedObject> SpaceObjectSpawner::SpaceObjectBuilder::SpawnNpc()
{
    const auto shipArch = Archetype::GetShip(npcTemplate.archetypeHash);

    pub::SpaceObj::ShipInfo si{};
    std::memset(&si, 0x0, sizeof(pub::SpaceObj::ShipInfo));

    si.flag = 1;

    si.system = system.value();
    si.pos = position.value();
    if (positionVariance.has_value())
    {
        si.pos.x += Random::i()->UniformFloat(-positionVariance.value(), positionVariance.value());
        si.pos.y += Random::i()->UniformFloat(-positionVariance.value(), positionVariance.value());
        si.pos.z += Random::i()->UniformFloat(-positionVariance.value(), positionVariance.value());
    }

    si.shipArchetype = npcTemplate.archetypeHash;
    si.orientation = rotation.value_or(Matrix::Identity());
    si.loadout = npcTemplate.loadoutHash;

    if (costumeOverride.has_value())
    {
        auto costume = costumeOverride.value();
        si.look1 = costume.head;
        si.look2 = costume.body;
        si.comm = costume.accessory[0];
    }
    else
    {
        si.look1 = CreateID("li_newscaster_head_gen_hat");
        si.look2 = CreateID("pl_female1_journeyman_body");
        si.comm = CreateID("comm_br_darcy_female");
    }

    si.pilotVoice = voiceOverride.value_or(CreateID("pilot_f_leg_f01a"));

    if (health.has_value())
    {
        if (healthRelative)
        {
            if (health.value() == 1.0f)
            {
                si.health = -1;
            }
            else if (health.value() <= 0.0f)
            {
                throw NpcLoadingException("Attempted to spawn an NPC with zero or negative health.");
            }
            else
            {
                si.health = static_cast<uint>(shipArch->hitPoints / health.value());
            }
        }
        else
        {
            if (health.value() <= 0.0f)
            {
                throw NpcLoadingException("Attempted to spawn an NPC with zero or negative health.");
            }

            si.health = static_cast<uint>(health.value());
        }
    }
    else
    {
        si.health = -1;
    }

    si.level = npcTemplate.level;

    FmtStr scannerName(0, nullptr);
    scannerName.begin_mad_lib(0);
    scannerName.end_mad_lib();

    // Define the string used for the pilot name. The example
    // below shows the use of multiple part names.
    FmtStr pilotName(0, nullptr);
    pilotName.begin_mad_lib(16163); // ids of "%s0 %s1"
    if (name.has_value())
    {
        const auto [firstName, secondName] = name.value();
        pilotName.append_string(firstName);

        if (secondName != 0)
        {
            pilotName.append_string(secondName);
        }
    }
    else
    {
        uint firstName = defaultNames[Random::i()->Uniform(0u, defaultNames.size() - 1)];
        uint secondName = defaultNames[Random::i()->Uniform(0u, defaultNames.size() - 1)];
        pilotName.append_string(firstName);  // ids that replaces %s0
        pilotName.append_string(secondName); // ids that replaces %s1
    }
    pilotName.end_mad_lib();

    pub::AI::SetPersonalityParams personalityParams;
    if (npcTemplate.pilotHash)
    {
        if (auto personality = PersonalityHelper::i()->Get(npcTemplate.pilot); personality.has_value())
        {
            personalityOverride.emplace(personality.value());
        }
    }

    if (personalityOverride.has_value())
    {
        personalityParams.personality = *personalityOverride.value();
    }

    personalityParams.stateGraph = pub::StateGraph::get_state_graph("FIGHTER", pub::StateGraph::TYPE_STANDARD);
    personalityParams.stateId = true;

    if (reputation.has_value())
    {
        uint r;
        pub::Reputation::GetReputationGroup(r, reputation.value().c_str());
        if (r)
        {
            pub::Reputation::Alloc(si.rep, scannerName, pilotName);
            pub::Reputation::SetAffiliation(si.rep, r);
        }
    }

    uint spaceObj = 0;
    pub::SpaceObj::Create(spaceObj, si);

    if (!spaceObj)
    {
        return {};
    }

    // Add the personality to the space obj
    pub::AI::SubmitState(spaceObj, &personalityParams);

    auto spawnedObject = std::make_shared<SpawnedObject>();
    spawnedObject->obj =
        static_cast<CSimple*>(CObject::Find(spaceObj, CObject::Class::CSHIP_OBJECT)); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

    spawnedObject->spaceObj = spaceObj;

    i()->spawnedObjects.emplace_back(spawnedObject);
    return spawnedObject;
}

std::weak_ptr<SpawnedObject> SpaceObjectSpawner::SpaceObjectBuilder::SpawnSolar()
{
    const auto solarArch = Archetype::GetSolar(npcTemplate.archetypeHash);

    pub::SpaceObj::SolarInfo si{};
    si.flag = 4;

    // Prepare the settings for the space object
    si.archId = npcTemplate.archetypeHash;
    si.loadoutId = npcTemplate.loadoutHash;
    si.hitPointsLeft = 1000;
    si.systemId = system.value();
    si.orientation = rotation.value_or(Matrix::Identity());

    si.costume =
        costumeOverride.value_or(Costume(CreateID("benchmark_male_head"), CreateID("benchmark_male_body"))); // NOLINT(clang-diagnostic-c++20-extensions)
    si.voiceId = voiceOverride.value_or(CreateID("atc_leg_m01"));

    si.rep = MakeId(reputation.value_or("").c_str());

    auto nickname = Random::i()->UniformString<std::string>(32);
    strncpy_s(si.nickName, sizeof(si.nickName), nickname.c_str(), nickname.size());

    // Do we need to vary the starting position slightly? Useful when spawning multiple objects
    si.pos = position.value();
    if (positionVariance.has_value())
    {
        si.pos.x += Random::i()->UniformFloat(-positionVariance.value(), positionVariance.value());
        si.pos.y += Random::i()->UniformFloat(-positionVariance.value(), positionVariance.value());
        si.pos.z += Random::i()->UniformFloat(-positionVariance.value(), positionVariance.value());
    }

    si.dockWith = dockTo.value_or(0);

    // Define the string used for the scanner name.
    // Because this is empty, the solar_name will be used instead.
    FmtStr scannerName(0, nullptr);
    scannerName.begin_mad_lib(0);
    scannerName.end_mad_lib();

    // Define the string used for the solar name.
    FmtStr solarName(0, nullptr);
    solarName.begin_mad_lib(16163); // ids of "%s0 %s1"
    if (name.has_value())
    {
        auto [firstName, secondName] = name.value();
        solarName.append_string(firstName);
        if (secondName != 0)
        {
            solarName.append_string(secondName);
        }
    }
    solarName.end_mad_lib();

    // Set Reputation
    pub::Reputation::Alloc(si.rep, scannerName, solarName);

    if (reputation.has_value())
    {
        uint r;
        pub::Reputation::GetReputationGroup(r, reputation.value().c_str());
        pub::Reputation::SetAffiliation(si.rep, r);
    }

    // Spawn the solar object
    uint spaceId;
    CreateSolar(spaceId, si);

    pub::AI::SetPersonalityParams personalityParams;
    if (npcTemplate.pilotHash)
    {
        if (auto personality = PersonalityHelper::i()->Get(npcTemplate.pilot); personality.has_value())
        {
            personalityOverride.emplace(personality.value());
        }
    }

    if (personalityOverride.has_value())
    {
        personalityParams.personality = *personalityOverride.value();
    }

    personalityParams.stateGraph = pub::StateGraph::get_state_graph("STATION", pub::StateGraph::TYPE_STANDARD);
    personalityParams.stateId = true;

    pub::AI::SubmitState(spaceId, &personalityParams);

    if (health.has_value())
    {
        if (healthRelative)
        {
            if (health.value() <= 0.0f)
            {
                throw NpcLoadingException("Attempted to spawn an NPC with zero or negative health.");
            }

            pub::SpaceObj::SetRelativeHealth(spaceId, health.value());
        }
        else
        {
            if (health.value() <= 0.0f)
            {
                throw NpcLoadingException("Attempted to spawn an NPC with zero or negative health.");
            }

            pub::SpaceObj::SetRelativeHealth(spaceId, health.value() / solarArch->hitPoints);
        }
    }
    else
    {
        pub::SpaceObj::SetRelativeHealth(spaceId, 1.0f);
    }

    auto spawnedObject = std::make_shared<SpawnedObject>();
    spawnedObject->obj = static_cast<CSimple*>(CObject::Find(spaceId, CObject::Class::CSHIP_OBJECT)); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

    if (!spawnedObject->obj)
    {
        return {};
    }

    spawnedObject->spaceObj = spaceId;

    i()->spawnedObjects.emplace_back(spawnedObject);
    return spawnedObject;
}

void SpaceObjectSpawner::SpaceObjectBuilder::ValidateSpawn()
{
    if (!Archetype::GetSolar(npcTemplate.archetypeHash) && !Archetype::GetShip(npcTemplate.archetypeHash))
    {
        throw NpcLoadingException(std::format("Ship/Solar archetype not found while building NPC/Station: {}", npcTemplate.archetype));
    }

    if (!system.has_value())
    {
        throw NpcLoadingException("Attempting to spawn NPC/Station without a system set.");
    }

    if (!position.has_value())
    {
        throw NpcLoadingException("Attempting to spawn NPC/Station without a position set.");
    }

    if (!npcTemplate.loadoutHash)
    {
        throw NpcLoadingException("Attempting to spawn an NPC/Station without a loadout set.");
    }
}

void SpaceObjectSpawner::Destroy(ResourcePtr<SpawnedObject> object)
{
    auto ptr = object.Acquire();
    if (!ptr)
    {
        std::erase_if(spawnedObjects, [ptr](auto& spawned) { return ptr->spaceObj == spawned->spaceObj; });
        return;
    }

    pub::SpaceObj::Destroy(ptr->spaceObj, DestroyType::Fuse);
    std::erase_if(spawnedObjects, [ptr](auto& spawned) { return ptr->spaceObj == spawned->spaceObj; });
}

void SpaceObjectSpawner::Despawn(ResourcePtr<SpawnedObject> object)
{
    auto ptr = object.Acquire();
    if (!ptr)
    {
        std::erase_if(spawnedObjects, [ptr](auto& spawned) { return ptr->spaceObj == spawned->spaceObj; });
        return;
    }

    pub::SpaceObj::Destroy(ptr->spaceObj, DestroyType::Vanish);
    std::erase_if(spawnedObjects, [ptr](auto& spawned) { return ptr->spaceObj == spawned->spaceObj; });
}

SpaceObjectSpawner::SpaceObjectSpawner()
{
    std::array<std::string, 2> dirs = { "../DATA/MISSIONS/", "../DATA/CHAOS" };

    std::ranges::for_each(dirs,
                          [this](auto dir)
                          {
                              for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
                              {
                                  std::string path = entry.path().generic_string();
                                  if (!entry.is_regular_file() || entry.file_size() > UINT_MAX || path.find("npcships") == std::string::npos)
                                  {
                                      continue;
                                  }

                                  INI_Reader ini;
                                  if (!ini.open(path.c_str(), false))
                                  {
                                      continue;
                                  }

                                  while (ini.read_header())
                                  {
                                      if (!ini.is_header("NPCShipArch"))
                                      {
                                          continue;
                                      }

                                      NpcTemplate npc{};
                                      while (ini.read_value())
                                      {
                                          if (ini.is_value("nickname"))
                                          {
                                              npc.nickname = ini.get_value_string();
                                              npc.hash = CreateID(npc.nickname.c_str());
                                          }
                                          else if (ini.is_value("loadout"))
                                          {
                                              npc.loadout = ini.get_value_string();
                                              npc.loadoutHash = CreateID(npc.loadout.c_str());
                                          }
                                          else if (ini.is_value("pilot"))
                                          {
                                              npc.pilot = ini.get_value_string();
                                              npc.pilotHash = CreateID(npc.pilot.c_str());
                                          }
                                          else if (ini.is_value("ship_archetype"))
                                          {
                                              npc.archetype = ini.get_value_string();
                                              npc.archetypeHash = CreateID(npc.archetype.c_str());
                                          }
                                          else if (ini.is_value("level"))
                                          {
                                              std::string level = ini.get_value_string();
                                              if (level.length() < 2)
                                              {
                                                  continue;
                                              }

                                              level.erase(0, 1);
                                              npc.level = StringUtils::Cast<uint>(level);
                                          }
                                      }

                                      if (!npc.archetypeHash || !npc.pilotHash || !npc.loadoutHash || !npc.hash || !npc.level)
                                      {
                                          continue;
                                      }

                                      npcTemplates[npc.hash] = npc;
                                  }
                              }
                          });
}

SpaceObjectSpawner::~SpaceObjectSpawner()
{
    std::ranges::for_each(spawnedObjects, [](auto& obj) { pub::SpaceObj::Destroy(obj->spaceObj, DestroyType::Vanish); });
}
