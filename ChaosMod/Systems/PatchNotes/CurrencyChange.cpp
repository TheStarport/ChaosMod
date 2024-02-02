#include "PCH.hpp"

#include "Changes.hpp"

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
    goodHash = possibleGoods[Random::i()->Uniform(0u, possibleGoods.size())];

    auto good = GoodList_get()->find_by_id(goodHash);

    std::string name;
    if (good->type == GoodInfo::Type::Commodity || good->type == GoodInfo::Type::Equipment)
    {
        const auto equip = Archetype::GetEquipment(good->equipmentId);
        name = GetInfocardName(equip->idsName);
    }
    else if (good->type == GoodInfo::Type::Ship)
    {
        good = (GoodList_get()->find_by_id(good->hullGoodId));
        const auto equip = Archetype::GetShip(good->equipmentId);
        name = GetInfocardName(equip->idsName);
    }

    oldPrice = good->price;
    newPrice = AdjustField(good->price, static_cast<bool>(Random::i()->Uniform(0u, 1u)));

    description = std::format("{}: Base Price   {:.0f}  ->  {:.0f}", name, oldPrice, newPrice);
}

nlohmann::json CurrencyChange::ToJson() { return Change::ToJson(); }
void CurrencyChange::FromJson(nlohmann::basic_json<> json) { Change::FromJson(json); }

size_t CurrencyChange::GetEffectCount()
{
    if (possibleGoods.empty())
    {
        const auto goods = GoodList_get();
        auto& list = *goods->get_list();

        for (const GoodInfo* good : list)
        {
            if (good->type != GoodInfo::Type::Hull && good->price != 0.f && good->goodId && good->idsName)
            {
                possibleGoods.emplace_back(good->goodId);
            }
        }
    }

    return possibleGoods.size();
}
