#pragma once

#include <nlohmann/json.hpp>
#include <regex>

enum class ChangeType
{
    Gun = 1,
    GunAmmo,
    GunMotor,
    GunExplosion,
    Shield,
    Thruster,
    Mine,
    MineAmmo,
    Cm,
    CmAmmo,
    Ship,
    Currency,
    // Constant,
    // Music
};

class Change
{
    protected:
        virtual ~Change() = default;

        struct FieldData
        {
                std::optional<Clamp> clamp;
                bool inverted = false;
                bool nbb = false;
        };

        struct MissileMap
        {
                uint motorId;
                uint explosionId;
        };
        inline static std::map<uint, MissileMap> missileMap;

        enum class FieldType
        {
            Int = 1,
            Float,
            Bool,
        };

        struct EditableField
        {
                std::string name;
                void* ptr;
                FieldType type;
        };

        template <typename T>
        std::vector<EditableField> GetEditableFields(T* item)
        {
            std::vector<EditableField> fields;
            constexpr auto type = refl::reflect<T>();
            for_each(type.members, // NOLINT(readability-static-accessed-through-instance)
                     [&](auto member)
                     {
                         if constexpr (member.is_static || !member.is_writable)
                         {
                             return;
                         }

                         auto& ref = *item;
                         using FieldTypeT = std::remove_reference_t<decltype(member(ref))>;
                         constexpr auto isInt = std::is_same_v<FieldTypeT, int> || std::is_same_v<FieldTypeT, uint>;
                         constexpr auto isFloat = std::is_same_v<FieldTypeT, float>;
                         constexpr auto isBool = std::is_same_v<FieldTypeT, bool>;

                         static_assert(isInt || isFloat || isBool, "Stat fields must be ints, floats or bools.");

                         FieldType fieldType;
                         if constexpr (isInt)
                         {
                             fieldType = FieldType::Int;
                         }
                         else if (isFloat)
                         {
                             fieldType = FieldType::Float;
                         }
                         else if (isBool)
                         {
                             fieldType = FieldType::Bool;
                         }

                         auto& instance = member(ref);

                         fields.emplace_back(std::string(member.name.c_str()), &instance, fieldType);
                     });

            return fields;
        }

        template <typename T>
            requires std::is_integral_v<T> || std::is_floating_point_v<T>
        static T AdjustField(T existingValue, bool increase, Random& randomEngine, std::optional<std::pair<T, T>> minMax = std::nullopt)
        {
            const auto value = static_cast<float>(existingValue);
            const float percentage = value * 0.01f;

            auto percentageChange = randomEngine.Uniform(1, 300);
            if (!increase)
            {
                percentageChange = std::clamp(-percentageChange, -95, -1); // If lowering make sure we don't end up negative
            }

            T returnValue = static_cast<T>(value + percentage * static_cast<float>(percentageChange));
            if (minMax.has_value())
            {
                returnValue = std::clamp(returnValue, minMax.value().first, minMax.value().second);
            }

            return returnValue;
        }

        void SetChangeNameAndDescription(const EditableField* field, const FieldData& fieldData, std::string_view itemName,
            void* oldValue, void* newValue, bool isBuff);

    public:
        enum class ChangePositivity
        {
            Default,
            Boon,
            Nerf,
            Neither
        };

        ChangeType changeType;
        ChangePositivity positivity;
        std::string description;

        virtual void Apply() = 0;
        virtual void Revert() = 0;
        virtual void Generate(Random& randomEngine) = 0;

        virtual nlohmann::json ToJson()
        {
            nlohmann::json json = nlohmann::json::object();
            json["type"] = changeType;
            json["description"] = description;
            json["positivity"] = positivity;
            return json;
        }

        virtual void FromJson(nlohmann::basic_json<> json)
        {
            changeType = json["type"];
            description = json["description"];
            positivity = json["positivity"];
        }

        // Multiply the change value by the modifier, if applicable
        virtual void Multiply(float multiplier){};
};

class CurrencyChange : public Change
{
        uint goodHash = 0;
        float newPrice = 0.f;
        float oldPrice = 0.f;

        inline static std::vector<uint> possibleGoods;

    public:
        CurrencyChange();

        void Apply() override;
        void Revert() override;
        void Generate(Random& randomEngine) override;
        void Multiply(float multiplier) override;
        nlohmann::json ToJson() override;
        void FromJson(nlohmann::basic_json<> json) override;

        static size_t GetEffectCount();
};

template <ChangeType Type>
class EquipmentChange : public Change
{
        /*static_assert(Type != ChangeType::Music && Type != ChangeType::Constant && Type != ChangeType::Currency,
                      "Type provided to an equipment change was not equipment");*/

        // clang-format off
        using T = 
             std::conditional_t<(Type == ChangeType::Gun), Archetype::Gun,
             std::conditional_t<(Type == ChangeType::Thruster), Archetype::Thruster,
             std::conditional_t<(Type == ChangeType::GunAmmo), Archetype::Munition,
             std::conditional_t<(Type == ChangeType::CmAmmo), Archetype::CounterMeasure,
             std::conditional_t<(Type == ChangeType::MineAmmo), Archetype::Mine,
             std::conditional_t<(Type == ChangeType::Mine), Archetype::MineDropper,
             std::conditional_t<(Type == ChangeType::Cm), Archetype::CounterMeasureDropper,
             std::conditional_t<(Type == ChangeType::GunMotor), Archetype::MotorData,
             std::conditional_t<(Type == ChangeType::GunExplosion), Archetype::Explosion,
             std::conditional_t<(Type == ChangeType::Ship), Archetype::Ship,
             std::conditional_t<(Type == ChangeType::Shield), Archetype::ShieldGenerator, PVOID
            >>>>>>>>>>>;

        static constexpr Archetype::ClassType GetClassType()
        {
            using namespace Archetype;
            switch (Type)
            {
                case ChangeType::Gun: return ClassType::Gun;
                case ChangeType::GunAmmo: return ClassType::Munition;
                case ChangeType::Shield: return ClassType::ShieldGenerator;
                case ChangeType::Thruster: return ClassType::Thruster;
                case ChangeType::Mine: return ClassType::MineDropper;
                case ChangeType::MineAmmo: return ClassType::Mine;
                case ChangeType::Cm: return ClassType::CounterMeasureDropper;
                case ChangeType::CmAmmo: return ClassType::CounterMeasure;
                case ChangeType::Ship: return ClassType::Ship;
                default: return ClassType::Root;
            }
        }

        // clang-format on

        static_assert(!std::is_same_v<T, PVOID>, "Invalid type provided to equipment change or template has invalid configuration");

        uint hash = 0;
        std::string field;
        std::array<byte, 4> newData{};
        std::array<byte, 4> oldData{};

        static constexpr std::pair<std::vector<FieldData>, std::vector<uint>> GetFieldData()
        {
            std::vector<FieldData> fields;
            std::vector<uint> weights;
            for_each(refl::descriptor::type_descriptor<T>::members,
                     [&fields, &weights](auto member)
                     {
                         FieldData data;
                         static_assert(!member.is_static && member.is_writable, "Unprocessable field!");

                         if constexpr (refl::descriptor::has_attribute<Weight>(member))
                         {
                             auto weight = refl::descriptor::get_attribute<Weight>(member);
                             weights.push_back(weight.w);
                         }
                         else
                         {
                             weights.push_back(100);
                         }

                         if constexpr (refl::descriptor::has_attribute<Clamp>(member))
                         {
                             auto clamp = refl::descriptor::get_attribute<Clamp>(member);
                             data.clamp = clamp;
                         }

                         if constexpr (refl::descriptor::has_attribute<Inverted>(member))
                         {
                             data.inverted = true;
                         }

                         if constexpr (refl::descriptor::has_attribute<Nbb>(member))
                         {
                             data.nbb = true;
                         }

                         fields.push_back(data);
                     });
            return { fields, weights };
        }

        inline static std::vector<uint> ignoredShips = { CreateID("chaos_jesus"),
                                                         CreateID("chaos_jesus_extreme"),
                                                         CreateID("or_osiris"),
                                                         CreateID("ge_liner"),
                                                         CreateID("ge_miner"),
                                                         CreateID("msn_playership"),
                                                         CreateID("li_fighter_indestr"),
                                                         CreateID("li_fighter_King"),
                                                         CreateID("large_transport_m03"),
                                                         CreateID("mission02_burning_l_elite"),
                                                         CreateID("mission02_burning_transport"),
                                                         CreateID("debris_tosser_200"),
                                                         CreateID("ge_armored_nobay"),
                                                         CreateID("RTC_Lifeboat"),
                                                         CreateID("chaos_big_bertha") };

        std::any GetArchetypePtr()
        {
            std::any erasure;
            Archetype::Root* root = nullptr;
            if constexpr (Type == ChangeType::Ship)
            {
                root = Archetype::GetShip(hash);
            }
            else if constexpr (Type == ChangeType::GunMotor)
            {
                auto motor = const_cast<Archetype::MotorData*>(Archetype::GetMotor(hash));
                if (!motor)
                {
                    return {};
                }

                erasure = motor;
            }
            else if constexpr (Type == ChangeType::GunExplosion)
            {
                ID_String id;
                id.id = hash;
                auto explosion = Archetype::GetExplosion(id);
                if (!explosion)
                {
                    return {};
                }

                erasure = explosion;
            }
            else
            {
                root = Archetype::GetEquipment(hash);
            }

            using TPtr = std::add_pointer_t<T>;
            if constexpr (Type != ChangeType::GunExplosion && Type != ChangeType::GunMotor)
            {
                const auto equipment = dynamic_cast<TPtr>(root);
                if (!equipment || equipment->get_class_type() != GetClassType())
                {
                    return {};
                }

                erasure = equipment;
            }

            return erasure;
        }

        std::string GetEquipmentName(T* item)
        {
            if constexpr (Type == ChangeType::GunExplosion)
            {
                for (auto& [munition, missileData] : missileMap)
                {
                    if (missileData.explosionId == item->id)
                    {
                        return ChaosMod::GetInfocardName(Archetype::GetEquipment(munition)->idsName);
                    }
                }
            }
            else if constexpr (Type == ChangeType::GunMotor)
            {
                for (auto& [munition, missileData] : missileMap)
                {
                    if (missileData.motorId == item->archId)
                    {
                        return ChaosMod::GetInfocardName(Archetype::GetEquipment(munition)->idsName);
                    }
                }
            }
            else
            {
                return ChaosMod::GetInfocardName(item->idsName);
            }

            // Will never happen, but suppresses the warning I guess
            return "";
        }

    public:
        inline static std::vector<uint> possibleEquipment;

        EquipmentChange() { changeType = Type; }

        void Apply() override
        {
            const auto erasure = GetArchetypePtr();
            if (!erasure.has_value())
            {
                return;
            }

            constexpr auto type = refl::reflect<T>();
            for_each(type.members, // NOLINT(readability-static-accessed-through-instance)
                     [&](auto member)
                     {
                         if constexpr (member.is_static || !member.is_writable)
                         {
                             return;
                         }

                         if (const std::string name = member.name.c_str(); field != name)
                         {
                             return;
                         }

                         auto& equip = *std::any_cast<std::add_pointer_t<T>>(erasure);
                         auto& instance = member(equip);
                         memcpy_s(&instance, 4, newData.data(), newData.size());
                     });
        }

        void Revert() override
        {
            const auto erasure = GetArchetypePtr();
            if (!erasure.has_value())
            {
                return;
            }

            constexpr auto type = refl::reflect<T>();
            for_each(type.members, // NOLINT(readability-static-accessed-through-instance)
                     [&](auto member)
                     {
                         if constexpr (member.is_static || !member.is_writable)
                         {
                             return;
                         }

                         if (const std::string name = member.name.c_str(); field != name)
                         {
                             return;
                         }

                         auto& equip = *std::any_cast<std::add_pointer_t<T>>(erasure);
                         auto& instance = member(equip);
                         memcpy_s(&instance, 4, oldData.data(), oldData.size());
                     });
        }

        void Multiply(float multiplier) override
        {
            const auto erasure = GetArchetypePtr();
            ASSERT(erasure.has_value(), "Failed to get archetype pointer!!");

            auto item = std::any_cast<std::add_pointer_t<T>>(erasure);
            auto fields = GetEditableFields<T>(item);

            int index = -1;
            for (auto& field : fields)
            {
                index++;
                if (field.name != this->field)
                {
                    continue;
                }

                bool isBuff = false;
                if (field.type == FieldType::Float)
                {
                    float oldValue, newValue;
                    memcpy_s(&oldValue, sizeof(float), oldData.data(), oldData.size());
                    memcpy_s(&newValue, sizeof(float), newData.data(), newData.size());
                    const auto diff = (oldValue - newValue) * multiplier;
                    newValue = std::abs(oldValue - diff);
                    memcpy_s(newData.data(), newData.size(), &newValue, sizeof(float));
                    isBuff = oldValue < newValue;
                }
                else if (field.type == FieldType::Int)
                {
                    int oldValue, newValue;
                    memcpy_s(&oldValue, sizeof(int), oldData.data(), oldData.size());
                    memcpy_s(&newValue, sizeof(int), newData.data(), newData.size());
                    const auto diff = static_cast<int>((oldValue - newValue) * multiplier);
                    newValue = std::abs(oldValue - diff);
                    memcpy_s(newData.data(), newData.size(), &newValue, sizeof(int));
                    isBuff = oldValue < newValue;
                }

                const auto allFieldData = GetFieldData();
                auto& fieldData = allFieldData.first[index];

                auto name = GetEquipmentName(item);

                SetChangeNameAndDescription(&field, fieldData, name, oldData.data(), newData.data(), isBuff);
            }
        }

        void Generate(Random& randomEngine) override
        {
            hash = possibleEquipment[randomEngine.Uniform(0u, possibleEquipment.size() - 1)];

            const auto erasure = GetArchetypePtr();
            ASSERT(erasure.has_value(), "Failed to get archetype pointer!!");

            auto item = std::any_cast<std::add_pointer_t<T>>(erasure);

            auto fields = GetEditableFields<T>(item);
            static const auto [fieldData, weights] = GetFieldData();
            uint statIndex = UINT_MAX;
            EditableField* value = nullptr;
            while (statIndex == UINT_MAX)
            {
                statIndex = randomEngine.Weighted(weights.begin(), weights.end());
                value = &fields[statIndex];
                if constexpr (Type == ChangeType::GunAmmo)
                {
                    if (!item->motorId && (value->name == "seeker_fov_deg" || value->name == "seeker_range" || value->name == "max_angular_velocity" ||
                                           value->name == "seeker" || value->name == "detonation_dist"))
                    {
                        statIndex = UINT_MAX;
                    }
                }
            }

            memcpy_s(oldData.data(), oldData.size(), value->ptr, 4);

            std::string name;
            if constexpr (Type == ChangeType::GunExplosion)
            {
                for (auto& [munition, missileData] : missileMap)
                {
                    if (missileData.explosionId == item->id)
                    {
                        name = ChaosMod::GetInfocardName(Archetype::GetEquipment(munition)->idsName);
                        break;
                    }
                }
            }
            else if constexpr (Type == ChangeType::GunMotor)
            {
                for (auto& [munition, missileData] : missileMap)
                {
                    if (missileData.motorId == item->archId)
                    {
                        name = ChaosMod::GetInfocardName(Archetype::GetEquipment(munition)->idsName);
                        break;
                    }
                }
            }
            else
            {
                name = ChaosMod::GetInfocardName(item->idsName);
            }

            auto buff = static_cast<bool>(randomEngine.Uniform(0u, 1u));

            const auto& data = fieldData[statIndex];
            auto clamp = data.clamp;

            if (clamp.has_value())
            {
                const auto& c = clamp.value();
                if (*static_cast<float*>(value->ptr) >= c.max)
                {
                    buff = false;
                }
                else if (*static_cast<float*>(value->ptr) <= c.min)
                {
                    buff = true;
                }
            }
            else
            {
                clamp = Clamp(INT_MIN, INT_MAX);
            }

            if (value->type == FieldType::Float)
            {
                auto fp = *(float*)value->ptr;
                if (fp == 0.0f)
                {
                    fp = 2.0f;
                }
                else
                {
                    fp = std::clamp(AdjustField(fp, buff, randomEngine), clamp->min, clamp->max);
                }

                memcpy_s(newData.data(), newData.size(), &fp, 4);
            }
            else if (value->type == FieldType::Int)
            {
                int i = *(int*)value->ptr;
                if (i == 0)
                {
                    i = 2;
                }
                else
                {
                    i = std::clamp(AdjustField(i, buff, randomEngine), static_cast<int>(clamp->min), static_cast<int>(clamp->max));
                }

                memcpy_s(newData.data(), newData.size(), &i, 4);
            }
            else if (value->type == FieldType::Bool)
            {
                *static_cast<bool*>(value->ptr) = !*static_cast<bool*>(value->ptr);
                bool newState = *static_cast<bool*>(value->ptr);
                memcpy_s(newData.data(), newData.size(), &newState, 4);
            }

            SetChangeNameAndDescription(value, data, name, oldData.data(), newData.data(), buff);
            field = value->name;
        }

        nlohmann::json ToJson() override
        {
            auto json = Change::ToJson();

            json["hash"] = hash;
            json["field"] = field;
            json["newData"] = newData;
            json["oldData"] = oldData;

            return json;
        }

        void FromJson(nlohmann::basic_json<> json) override
        {
            Change::FromJson(json);

            hash = json["hash"];
            field = json["field"];
            newData = json["newData"];
            oldData = json["oldData"];
        }

        static size_t GetEffectCount()
        {
            using namespace Archetype;

            if (possibleEquipment.empty())
            {
                if constexpr (Type == ChangeType::Ship)
                {
                    const auto ships = reinterpret_cast<BinarySearchTree<Ship*>*>(0x63FCAC0);
                    for (auto node = ships->begin(); node != ships->end(); ++node)
                    {
                        if (std::ranges::find(ignoredShips, node->value->archId) != ignoredShips.end() || !node->value->idsName ||
                            node->value->maxNanobots == UINT_MAX || node->value->maxShieldBats == UINT_MAX ||
                            ChaosMod::GetInfocardName(node->value->idsName).empty())
                        {
                            continue;
                        }

                        possibleEquipment.emplace_back(node->key);
                    }
                }
                else if constexpr (Type == ChangeType::GunExplosion)
                {
                    const auto explosions = reinterpret_cast<BinarySearchTree<Explosion>*>(0x63FCF3C);
                    for (auto node = explosions->begin(); node != explosions->end(); ++node)
                    {
                        if (std::ranges::find_if(missileMap, [node](auto& map) { return map.second.explosionId == node->key; }) == missileMap.end())
                        {
                            continue;
                        }

                        possibleEquipment.emplace_back(node->key);
                    }
                }
                else if constexpr (Type == ChangeType::GunMotor)
                {
                    const auto motors = reinterpret_cast<BinarySearchTree<MotorData>*>(0x63FCA70);
                    for (auto node = motors->begin(); node != motors->end(); ++node)
                    {
                        if (std::ranges::find_if(missileMap, [node](auto& map) { return map.second.motorId == node->key; }) == missileMap.end())
                        {
                            continue;
                        }

                        possibleEquipment.emplace_back(node->key);
                    }
                }
                else
                {
                    const auto goodList = GoodList_get();

                    const auto equipment = reinterpret_cast<BinarySearchTree<Equipment*>*>(0x63FCAD4);
                    for (auto node = equipment->begin(); node != equipment->end(); ++node)
                    {
                        if (node->value->idsName == 0 || node->value->idsInfo == 0 || ChaosMod::GetInfocardName(node->value->idsName).empty())
                        {
                            continue;
                        }

                        if constexpr (Type == ChangeType::GunAmmo)
                        {
                            const auto munition = reinterpret_cast<Munition*>(node->value);
                            if (munition->motorId && munition->explosionArchId)
                            {
                                missileMap[node->key] = MissileMap{ munition->motorId, munition->explosionArchId };
                            }
                        }

                        if (node->value->get_class_type() == GetClassType() || !node->value->idsName)
                        {
                            auto goodId = Arch2Good(node->value->archId);
                            auto good = goodList->find_by_id(goodId);

                            if (!good || good->price == 0.0f)
                            {
                                continue;
                            }

                            possibleEquipment.emplace_back(node->key);
                        }
                    }
                }
            }

            return possibleEquipment.size();
        }
};
