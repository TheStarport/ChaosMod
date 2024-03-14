#include "PCH.hpp"

#include "Changes.hpp"

CurrencyChange::CurrencyChange() { changeType = ChangeType::Currency; }

void CurrencyChange::Apply()
{
    const auto goods = GoodList_get();
    const auto good = const_cast<GoodInfo*>(goods->find_by_id(goodHash));
    if (!good)
    {
        return;
    }

    good->price = newPrice;
}
void CurrencyChange::Revert()
{
    const auto goods = GoodList_get();
    const auto good = const_cast<GoodInfo*>(goods->find_by_id(goodHash));
    if (!good)
    {
        return;
    }

    good->price = oldPrice;
}

void CurrencyChange::Generate()
{
    goodHash = possibleGoods[Get<Random>()->Uniform(0u, possibleGoods.size() - 1)];

    auto good = GoodList_get()->find_by_id(goodHash);

    std::string name;
    if (good->type == GoodInfo::Type::Commodity || good->type == GoodInfo::Type::Equipment)
    {
        const auto equip = Archetype::GetEquipment(good->equipmentId);
        name = ChaosMod::GetInfocardName(equip->idsName);
    }
    else if (good->type == GoodInfo::Type::Ship)
    {
        good = (GoodList_get()->find_by_id(good->hullGoodId));
        const auto equip = Archetype::GetShip(good->equipmentId);
        name = ChaosMod::GetInfocardName(equip->idsName);
    }

    oldPrice = good->price;
    // clang-format off
    newPrice = AdjustField(good->price, static_cast<bool>(Get<Random>()->Uniform(0u, 1u)), {{ 1.f, 999999.f }});
    // clang-format on

    description = std::format("~ {}: Base Price   {:.0f}  ->  {:.0f}", name, oldPrice, newPrice);
    positivity = ChangePositivity::Neither;
}

nlohmann::json CurrencyChange::ToJson()
{
    auto json = Change::ToJson();
    json["goodHash"] = goodHash;
    json["oldPrice"] = oldPrice;
    json["newPrice"] = newPrice;
    return json;
}
void CurrencyChange::FromJson(nlohmann::basic_json<> json)
{
    Change::FromJson(json);

    goodHash = json["goodHash"];
    oldPrice = json["oldPrice"];
    newPrice = json["newPrice"];
}

size_t CurrencyChange::GetEffectCount()
{
    if (possibleGoods.empty())
    {
        const auto goods = GoodList_get();
        auto& list = *goods->get_list();

        for (const GoodInfo* good : list)
        {
            // If it doesn't have a name, or is an engine/powerplant, don't list it
            if (ChaosMod::GetInfocardName(good->idsName).empty() ||
                _strcmpi(good->itemIcon, R"(equipment\models\commodities\nn_icons\EQUIPICON_engine.3db)") == 0)
            {
                continue;
            }

            if (good->type != GoodInfo::Type::Hull && good->price != 0.f && good->goodId && good->idsName)
            {
                possibleGoods.emplace_back(good->goodId);
            }
        }
    }

    return possibleGoods.size();
}
