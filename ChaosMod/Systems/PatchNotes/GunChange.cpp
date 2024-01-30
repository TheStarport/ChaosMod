#include "PCH.hpp"

#include "Patch.hpp"

#include <refl.hpp>
#include <regex>

void GunChange::Apply()
{
    const auto equipment = dynamic_cast<Archetype::Gun*>(Archetype::GetEquipment(hash));
    if (!equipment || equipment->get_class_type() != Archetype::ClassType::Gun)
    {
        return;
    }

    constexpr auto type = refl::reflect<Archetype::Gun>();
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

void GunChange::Revert()
{
    const auto equipment = dynamic_cast<Archetype::Gun*>(Archetype::GetEquipment(hash));
    if (!equipment || equipment->get_class_type() != Archetype::ClassType::Gun)
    {
        return;
    }

    constexpr auto type = refl::reflect<Archetype::Gun>();
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

                 memcpy_s(&instance, 4, oldData.data(), oldData.size());
             });
}

void GunChange::Generate()
{
    if (possibleWeapons.empty())
    {
        const auto goodList = GoodList_get();

        const auto equipment = reinterpret_cast<BinarySearchTree<Archetype::Equipment*>*>(0x63FCAD8);
        equipment->TraverseTree(
            [this, &goodList](auto pair)
            {
                if (pair.second->get_class_type() == Archetype::ClassType::Gun)
                {
                    auto goodId = Arch2Good(pair.second->archId);
                    auto good = goodList->find_by_id(goodId);
                    if (!good || good->price == 0.0f)
                    {
                        return;
                    }

                    possibleWeapons.emplace_back(goodId);
                }
            });
    }

    const auto randomGun = possibleWeapons[Random::i()->Uniform(0u, possibleWeapons.size() - 1)];

    const auto gun = reinterpret_cast<Archetype::Gun*>(Archetype::GetEquipment(randomGun));
    auto fields = GetEditableFields(gun);

    const auto [f, ptr, type] = fields[Random::i()->Uniform(0u, fields.size() - 1)];

    memcpy_s(oldData.data(), oldData.size(), ptr, 4);

    auto name = GetInfocardName(gun->idsName);

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
            fp = 5.f;
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
            i = 5.f;
        }
        else
        {
            i = AdjustField(i, buff);
        }

        description = std::format("{}: {}   {}  ->  {}", name, newFieldName, *(int*)ptr, i);
        memcpy_s(newData.data(), newData.size(), &i, 4);
    }

    field = f;
    hash = randomGun;
}

nlohmann::json GunChange::ToJson() { return Change::ToJson(); }
void GunChange::FromJson(nlohmann::basic_json<> json) { Change::FromJson(json); }
