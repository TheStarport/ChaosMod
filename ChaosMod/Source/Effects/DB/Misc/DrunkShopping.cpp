#include "Effects/ActiveEffect.hpp"
#include "FLCore/Common/Globals.hpp"
#include "FLCore/FLCoreServer.h"

class DrunkShopping final : public ActiveEffect
{
        std::vector<uint> possibleWeapons;

        void Begin() override
        {
            if (possibleWeapons.empty())
            {
                const auto goodList = GoodList_get();

                for (auto equip = GameData::equipment.begin(); equip != GameData::equipment.end(); ++equip)
                {
                    if (equip->second->get_class_type() == Archetype::ClassType::Gun || equip->second->get_class_type() == Archetype::ClassType::Shield)
                    {
                        auto goodId = Arch2Good(equip->second->archId);
                        auto good = goodList->find_by_id(goodId);
                        if (!good || good->price == 0.0f)
                        {
                            continue;
                        }

                        possibleWeapons.emplace_back(goodId);
                    }
                }
            }

            for (int i = 0; i < 5; i++)
            {
                auto cargo = possibleWeapons[Get<Random>()->Uniform(0u, possibleWeapons.size() - 1)];
                pub::Player::AddCargo(1, cargo, 1, 1.0f, false);
            }
        }

    public:
        explicit DrunkShopping(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DrunkShopping, {
    .effectName = "Drunk Shopping",
    .description = "How much did you drink last night? And what is this large bill to Universal Shipping? You better check your cargo hold for what you bought last night!",
    .category = EffectType::Misc,
    .timingModifier = 0.0f,
    .isTimed = false
});