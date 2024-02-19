#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class DrunkShopping final : public ActiveEffect
{
        std::vector<uint> possibleWeapons;

        void Begin() override
        {
            if (possibleWeapons.empty())
            {
                const auto goodList = GoodList_get();

                auto* equipment = reinterpret_cast<BinarySearchTree<Archetype::Equipment*>*>(0x63FCAD4);
                for (auto equip = equipment->begin(); equip != equipment->end(); ++equip)
                {
                    if (equip->value->get_class_type() == Archetype::ClassType::Gun || equip->value->get_class_type() == Archetype::ClassType::Shield)
                    {
                        auto goodId = Arch2Good(equip->value->archId);
                        auto good = goodList->find_by_id(goodId);
                        if (!good || good->price == 0.0f)
                        {
                            return;
                        }

                        possibleWeapons.emplace_back(goodId);
                    }
                }
            }

            for (int i = 0; i < 5; i++)
            {
                auto cargo = possibleWeapons[Random::i()->Uniform(0u, possibleWeapons.size() - 1)];
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