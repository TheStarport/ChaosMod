#include "PCH.hpp"

#include "SpaceObjectSpawner.hpp"

#include "AssetTracker.hpp"
#include "Constants.hpp"
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

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithArchetype(const uint archetype)
{
    npcTemplate.archetypeHash = archetype;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithLoadout(const std::string& loadout)
{
    npcTemplate.loadout = loadout;
    npcTemplate.loadoutHash = CreateID(loadout.c_str());
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithLoadout(uint loadout)
{
    npcTemplate.loadoutHash = loadout;
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

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithPersonality(pub::AI::Personality& personality)
{
    personalityOverride = &personality;
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

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithName(FmtStr& scannerName, FmtStr& pilotName)
{
    names = { scannerName, pilotName };
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithReputation(const std::string& rep)
{
    reputation = rep;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithReputation(uint affiliation)
{
    this->affiliation = affiliation;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithDockTo(uint base)
{
    dockTo = base;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithFuse(const std::string& fuse)
{
    this->fuse = fuse;
    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithRandomNpc()
{
    auto& templates = i()->npcTemplates;
    auto el = templates.begin();
    std::advance(el, Random::i()->Uniform(0u, templates.size() - 1));

    npcTemplate = el->second;

    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithRandomReputation()
{
    auto* groups = reinterpret_cast<FlMap<Reputation::RepGroup>*>(0x64018EC);

    const uint randomGroupIndex = Random::i()->Uniform(0u, groups->size() - 1);
    uint currentIndex = 0;
    for (auto listItem = groups->begin(); listItem != groups->end(); ++listItem, ++currentIndex)
    {
        if (currentIndex == randomGroupIndex)
        {
            reputation = listItem.value()->name;
            break;
        }
    }

    return *this;
}

SpaceObjectSpawner::SpaceObjectBuilder& SpaceObjectSpawner::SpaceObjectBuilder::WithRandomName()
{
    struct NameRange
    {
            std::pair<uint, uint> firstName;
            std::pair<uint, uint> lastName;
    };

    // clang-format off
    constexpr std::array<NameRange, 5> nameLists = {
        {
            // Liberty
            {
                { 226608, 226952 },
                { 227008, 227307 }
            },
            // Kusari
        {
            { 228708, 228890 },
            { 228908, 229207 }
            },
            // Hispania
            {
            { 229208, 229340 },
            { 229408, 229460 }
            },
            // Rheinland
            {
            { 228008, 228407 },
            { 228408, 228663 }
            },
            // Bretonia
            {
            { 227308, 227575 },
            { 227708, 228007 }
            },
        }
    };
    // clang-format on
    const auto& [firstName, lastName] = nameLists[Random::i()->Uniform(0u, nameLists.size() - 1)];

    name = {
        Random::i()->Uniform(firstName.first, firstName.second),
        Random::i()->Uniform(lastName.first, lastName.second),
    };

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

    using T = ResourcePtr<SpawnedObject>;
    auto response = isNpc ? T{ SpawnNpc(), validateExisting } : T{ SpawnSolar(), validateExisting };

    if (fuse.has_value())
    {
        if (const auto obj = response.Acquire())
        {
            Utils::LightFuse(Utils::GetInspect(obj->spaceObj), CreateID(fuse.value().c_str()), 0.0f, 5.0f, 0.0f);
        }
    }

    return response;
}

std::weak_ptr<SpawnedObject> SpaceObjectSpawner::SpaceObjectBuilder::SpawnNpc()
{
    const auto shipArch = Archetype::GetShip(npcTemplate.archetypeHash);

    pub::SpaceObj::ShipInfo si{};
    std::memset(&si, 0x0, sizeof(pub::SpaceObj::ShipInfo)); // NOLINT

    si.flag = AssetTracker::GetLastCreationFlag();

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
    if (names.has_value())
    {
        auto [scanner, pilot] = names.value();
        scannerName = scanner;
        pilotName = pilot;
    }
    else if (name.has_value())
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

    pub::Reputation::Alloc(si.rep, scannerName, pilotName);
    if (affiliation.has_value())
    {
        pub::Reputation::SetAffiliation(si.rep, affiliation.value());
    }
    else if (reputation.has_value())
    {
        uint r;
        pub::Reputation::GetReputationGroup(r, reputation.value().c_str());
        if (r)
        {
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

    if (!Loadout::Get(npcTemplate.loadoutHash))
    {
        throw NpcLoadingException(std::format("{} ({}) loadout was not found while loading NPC.", npcTemplate.loadout, npcTemplate.loadoutHash));
    }
}

void SpaceObjectSpawner::CreateNewLoadout(const std::string& nickname, const std::vector<LoadoutItem>& items)
{
    std::string block = std::format("[Loadout]\nnickname = {}\n", nickname);
    std::ranges::for_each(items,
                          [&block](auto& item)
                          {
                              if (item.count > 0)
                              {
                                  block += std::format("cargo = {}, {}\n", item.nickname, item.count);
                              }
                              else
                              {
                                  if (item.mountedHp.has_value())
                                  {
                                      block += std::format("equip = {}, {}\n", item.nickname, item.mountedHp.value());
                                  }
                                  else
                                  {
                                      block += std::format("equip = {}\n", item.nickname);
                                  }
                              }
                          });

    INI_Reader ini;
    if (!ini.open_memory(block.data(), block.size()))
    {
        Log("Failed to open in-memory INI loadout entry");
        return;
    }

    const auto id = CreateID(nickname.c_str());
    struct DataPair
    {
            uint id;
            ID_String unk;
    };

    DataPair v25 = { id, {} };

    using CreateLoadout = int(__thiscall*)(void* thisPtr, char** unk, DataPair* unk2);
    const auto sub6311Db0 = reinterpret_cast<CreateLoadout>(reinterpret_cast<DWORD>(GetModuleHandleA("common.dll")) + 0xB1DB0);

    const auto unkClass = reinterpret_cast<PDWORD>(reinterpret_cast<DWORD>(GetModuleHandleA("common.dll")) + 0x19D2A8);
    static char* output;
    sub6311Db0(unkClass, &output, &v25);
    const auto v3 = output + 16;
    *reinterpret_cast<DWORD*>(output + 16) = id;

    ini.find_header("Loadout");
    while (ini.read_value())
    {
        if (ini.is_value("equip"))
        {
            EquipDesc v8;
            auto* vec = reinterpret_cast<st6::vector<EquipDesc>*>(v3 + 4);
            vec->insert(*reinterpret_cast<st6::vector<EquipDesc>::iterator*>(v3 + 12), 1, v8);
            using ReadEquipLine = bool (*)(INI_Reader&, EquipDesc*);
            reinterpret_cast<ReadEquipLine>(reinterpret_cast<DWORD>(Loadout::ReadEquipLine))(
                ini, reinterpret_cast<EquipDesc*>(*reinterpret_cast<DWORD*>(v3 + 12) - 32));
            const auto shieldId = reinterpret_cast<SubObjectID::ShieldIdMaker*>(v3 + 20)->CreateShieldID();
            reinterpret_cast<EquipDesc*>(*reinterpret_cast<DWORD*>(v3 + 12) - 32)->set_id(shieldId);
        }
        else if (ini.is_value("cargo"))
        {
            EquipDesc v28;
            Loadout::ReadCargoLine(ini, v28);

            bool v18 = arch_is_combinable(CreateID(ini.get_value_string(0)));
            if (!v28.is_internal())
            {
                v18 = false;
            }
            if (!reinterpret_cast<EquipDescVector*>(v3 + 4)->add_equipment_item(v28, v18))
            {
                const auto shieldId = reinterpret_cast<SubObjectID::ShieldIdMaker*>(v3 + 20)->CreateShieldID();
                reinterpret_cast<EquipDesc*>(*reinterpret_cast<DWORD*>(v3 + 12) - 32)->set_id(shieldId);
            }
        }
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
