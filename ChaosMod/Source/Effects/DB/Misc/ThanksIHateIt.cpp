#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class ThanksIHateIt final : public ActiveEffect
{
        std::vector<uint> possibleCargo;

        void Begin() override
        {
            const auto ship = Utils::GetCShip();
            auto remaining = ship->get_cargo_hold_remaining();
            if (remaining < 1.0f)
            {
                return;
            }

            if (possibleCargo.empty())
            {
                auto* equipment = reinterpret_cast<BinarySearchTree<Archetype::Equipment*>*>(0x63FCAD4);
                for (auto equip = equipment->begin(); equip != equipment->end(); ++equip)
                {
                    if (equip->value->get_class_type() == Archetype::ClassType::Commodity && equip->value->volume > 0.5f)
                    {
                        possibleCargo.emplace_back(Arch2Good(equip->value->archId));
                    }
                }
            }

            while (remaining > 1.0f)
            {
                auto cargo = possibleCargo[Get<Random>()->Uniform(0u, possibleCargo.size() - 1)];
                pub::Player::AddCargo(1, cargo, 1, 1.0f, false);
                remaining -= 1.0f;
            }
        }

    public:
        explicit ThanksIHateIt(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(ThanksIHateIt, {
    .effectName = "Thank's I Hate It",
    .description = "Check your cargo hold. I bet this goodie bag is what you always wanted.!",
    .category = EffectType::Misc,
    .timingModifier = 0.0f,
    .isTimed = false
});