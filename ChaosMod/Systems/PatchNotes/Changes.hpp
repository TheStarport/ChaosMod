#pragma once

#include <nlohmann/json.hpp>
#include <regex>

enum class ChangeType
{
    Gun = 1,
    GunAmmo,
    // GunMotor,
    // GunExplosion,
    Shield,
    Thruster,
    Mine,
    MineAmmo,
    Cm,
    CmAmmo,
    Ship,
    // Currency,
    // Constant,
    // Music
};

class Change
{
    protected:
        virtual ~Change() = default;

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
        static T AdjustField(T existingValue, bool increase)
        {
            const auto value = static_cast<float>(existingValue);
            const float percentage = value * 0.01f;

            auto percentageChange = Random::i()->Uniform(1, 300);
            if (!increase)
            {
                percentageChange = std::clamp(-percentageChange, -95, -1); // If lowering make sure we don't end up negative
            }

            return static_cast<T>(value + percentage * static_cast<float>(percentageChange));
        }

    public:
        ChangeType changeType;
        std::string description;

        virtual void Apply() = 0;
        virtual void Revert() = 0;
        virtual void Generate() = 0;
        virtual size_t GetEffectCount() = 0;

        virtual nlohmann::json ToJson()
        {
            nlohmann::json json = nlohmann::json::object();
            json["type"] = changeType;
            json["description"] = description;
            return json;
        };

        virtual void FromJson(nlohmann::basic_json<> json)
        {
            changeType = json["type"];
            description = json["description"];
        }
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
             //std::conditional_t<(Type == ChangeType::GunMotor), Archetype::MotorData,
             //std::conditional_t<(Type == ChangeType::GunExplosion), Archetype::Explosion,
             std::conditional_t<(Type == ChangeType::Ship), Archetype::Ship,
             std::conditional_t<(Type == ChangeType::Shield), Archetype::ShieldGenerator, PVOID
            >>>>>>>>>;

        static constexpr Archetype::ClassType GetClassType()
        {
            using namespace Archetype;
            switch (Type)
            {
                
                case ChangeType::Gun: return ClassType::Gun;
                case ChangeType::GunAmmo: return ClassType::Munition;;
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

        static constexpr std::vector<uint> GetFieldWeights()
        {
            std::vector<uint> fields;
            for_each(refl::descriptor::type_descriptor<T>::members,
                     [&fields](auto member)
                     {
                         if constexpr (!member.is_static && member.is_writable && refl::descriptor::has_attribute<Weight>(member))
                         {
                             auto weight = refl::descriptor::get_attribute<Weight>(member);
                             fields.push_back(weight.w);
                         }
                         else
                         {
                             fields.push_back(3);
                         }
                     });
            return fields;
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

    public:
        inline static std::vector<uint> possibleEquipment;

        EquipmentChange()
        {
            changeType = Type;

            using namespace Archetype;

            if (possibleEquipment.empty())
            {
                if constexpr (Type == ChangeType::Ship)
                {
                    const auto ships = reinterpret_cast<BinarySearchTree<Ship*>*>(0x63FCAC0);
                    ships->TraverseTree(
                        [this](std::pair<uint, Ship*> pair)
                        {
                            if (std::ranges::find(ignoredShips, pair.second->archId) != ignoredShips.end())
                            {
                                return;
                            }

                            possibleEquipment.emplace_back(pair.first);
                        });
                }
                else
                {
                    const auto goodList = GoodList_get();

                    const auto equipment = reinterpret_cast<BinarySearchTree<Equipment*>*>(0x63FCAD8);
                    equipment->TraverseTree(
                        [this, &goodList](std::pair<uint, Equipment*> pair)
                        {
                            if (pair.second->get_class_type() == GetClassType())
                            {
                                auto goodId = Arch2Good(pair.second->archId);
                                auto good = goodList->find_by_id(goodId);
                                if (!good || good->price == 0.0f)
                                {
                                    return;
                                }

                                possibleEquipment.emplace_back(pair.first);
                            }
                        });
                }
            }
        }

        void Apply() override
        {
            Archetype::Root* root;
            if constexpr (Type == ChangeType::Ship)
            {
                root = Archetype::GetShip(hash);
            }
            else
            {
                root = Archetype::GetEquipment(hash);
            }

            using TPtr = std::add_pointer_t<T>;
            const auto equipment = dynamic_cast<TPtr>(root);
            if (!equipment || equipment->get_class_type() != GetClassType())
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

                         auto& gun = *equipment;
                         auto& instance = member(gun);

                         memcpy_s(&instance, 4, newData.data(), newData.size());
                     });
        }
        void Revert() override
        {
            Archetype::Root* root;
            if constexpr (Type == ChangeType::Ship)
            {
                root = Archetype::GetShip(hash);
            }
            else
            {
                root = Archetype::GetEquipment(hash);
            }

            using TPtr = std::add_pointer_t<T>;
            const auto equipment = dynamic_cast<TPtr>(root);
            if (!equipment || equipment->get_class_type() != GetClassType())
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

                         auto& equip = *equipment;
                         auto& instance = member(equip);

                         memcpy_s(&instance, 4, oldData.data(), oldData.size());
                     });
        }
        void Generate() override
        {
            const auto randomEquipment = possibleEquipment[Random::i()->Uniform(0u, possibleEquipment.size() - 1)];

            Archetype::Root* root;
            if constexpr (Type == ChangeType::Ship)
            {
                root = Archetype::GetShip(randomEquipment);
            }
            else
            {
                root = Archetype::GetEquipment(randomEquipment);
            }

            using TPtr = std::add_pointer_t<T>;
            const auto equipment = dynamic_cast<TPtr>(root);
            if (!equipment || equipment->get_class_type() != GetClassType())
            {
                return;
            }

            auto fields = GetEditableFields(equipment);

            static const auto fieldWeights = GetFieldWeights();

            const uint statIndex = Random::i()->Weighted(fieldWeights.begin(), fieldWeights.end());

            const auto [f, ptr, type] = fields[statIndex];

            memcpy_s(oldData.data(), oldData.size(), ptr, 4);

            auto name = GetInfocardName(equipment->idsName);

            std::string newFieldName = std::regex_replace(f, std::regex("([a-z])([A-Z])"), "$1 $2");
            bool lastCharWasSpace = false;
            for (uint i = 0; i < newFieldName.size(); ++i)
            {
                if (lastCharWasSpace || i == 0)
                {
                    newFieldName[i] = static_cast<char>(std::toupper(newFieldName[i]));
                    lastCharWasSpace = false;
                    continue;
                }
                if (newFieldName[i] == ' ')
                {
                    lastCharWasSpace = true;
                }
            }

            const auto buff = static_cast<bool>(Random::i()->Uniform(0u, 1u));

            if (type == FieldType::Float)
            {
                auto fp = *(float*)ptr;
                if (fp == 0.0f)
                {
                    fp = 2.0f;
                }
                else
                {
                    fp = AdjustField(fp, buff);
                }

                description = std::format("{}: {}   {:.2f}  ->  {:.2f}", name, newFieldName, *(float*)ptr, fp);
                memcpy_s(newData.data(), newData.size(), &fp, 4);
            }
            else if (type == FieldType::Int)
            {
                int i = *(int*)ptr;
                if (i == 0)
                {
                    i = 2;
                }
                else
                {
                    i = AdjustField(i, buff);
                }

                description = std::format("{}: {}   {}  ->  {}", name, newFieldName, *(int*)ptr, i);
                memcpy_s(newData.data(), newData.size(), &i, 4);
            }

            field = f;
            hash = randomEquipment;
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

        size_t GetEffectCount() { return possibleEquipment.size(); }
};
