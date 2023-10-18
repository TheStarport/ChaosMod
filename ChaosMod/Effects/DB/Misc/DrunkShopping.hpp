#pragma once
#include "Effects/ActiveEffect.hpp"

class DrunkShopping final : public ActiveEffect
{
        std::vector<uint> possibleWeapons;

        void Begin() override
        {
            if (possibleWeapons.empty())
            {
                const auto goodList = GoodList_get();

                const auto equipment = reinterpret_cast<BinarySearchTree<Archetype::Equipment*>*>(0x63FCAD8);
                equipment->TraverseTree(
                    [this, &goodList](auto pair)
                    {
                        if (pair.second->get_class_type() == Archetype::ClassType::Gun || pair.second->get_class_type() == Archetype::ClassType::Shield)
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

            for (int i = 0; i < 5; i++)
            {
                auto cargo = possibleWeapons[Random::i()->Uniform(0u, possibleWeapons.size() - 1)];
                pub::Player::AddCargo(1, cargo, 1, 1.0f, false);
            }
        }

    public:
        DefEffectInfo("Drunk Shopping", 0.0f, EffectType::Misc);
};
