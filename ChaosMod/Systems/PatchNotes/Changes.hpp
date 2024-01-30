#pragma once

#include <nlohmann/json.hpp>

enum class ChangeType
{
    Gun = 1,
};

class Change
{
    protected:
        virtual ~Change() = default;

        enum class FieldType
        {
            Int = 1,
            Float,
            String,
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
                         constexpr auto isInt = std::is_same_v<FieldTypeT, int>;
                         constexpr auto isFloat = std::is_same_v<FieldTypeT, float>;

                         static_assert(isInt || isFloat, "Stat fields must be ints or floats.");

                         FieldType fieldType;
                         if constexpr (isInt)
                         {
                             fieldType = FieldType::Int;
                         }
                         else if (isFloat)
                         {
                             fieldType = FieldType::Float;
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
            const float value = static_cast<float>(existingValue);
            const float percentage = value * 0.01f;

            auto percentageChange = Random::i()->Uniform(1, 300);
            if (!increase)
            {
                percentageChange = std::clamp(-percentageChange, -95, -1); // If lowering make sure we don't end up negative
            }

            return value + percentage * percentageChange;
        }

    public:
        ChangeType changeType;
        std::string description;

        virtual void Apply() = 0;
        virtual void Revert() = 0;
        virtual void Generate() = 0;

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

class GunChange final : public Change
{
        uint hash = 0;
        std::string field;
        std::array<byte, 4> newData{};
        std::array<byte, 4> oldData{};

    public:
        inline static std::vector<uint> possibleWeapons;

        GunChange() { changeType = ChangeType::Gun; }

        void Apply() override;
        void Revert() override;
        void Generate() override;

        nlohmann::json ToJson() override;
        void FromJson(nlohmann::basic_json<> json) override;
};
