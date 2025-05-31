#include "PCH.hpp"

#include "Effects/PersistentEffect.hpp"

class DontStarve final : public PersistentEffect
{
        void Begin() override
        {
            PersistentEffect::Begin();

            auto* food = Archetype::GetEquipment(CreateID("commodity_food"));
            if (food->get_class_type() != Archetype::ClassType::Commodity)
            {
                Log("commodity_food is not a commodity");
                return;
            }

            // Decay 1 unit of food every 60 seconds
            dynamic_cast<Archetype::Commodity*>(food)->decayPerSecond = 1;
            dynamic_cast<Archetype::Commodity*>(food)->idsInfo = 458876;

            const auto good = const_cast<GoodInfo*>(GoodList::find_by_nickname("commodity_food"));
            good->idsInfo = 458876;

            pub::Player::AddCargo(1, CreateID("commodity_luxury_food"), 5, 1.0f, true);

            FmtStr caption(0, nullptr);
            caption.begin_mad_lib(458874);
            caption.end_mad_lib();

            FmtStr message(0, nullptr);
            message.begin_mad_lib(458875);
            message.end_mad_lib();

            pub::Player::PopUpDialog(1, caption, message, static_cast<int>(PopupDialog::CenterOk));
        }

        void End() override { PersistentEffect::End(); }

        std::array<uint, 2> possibleFuel = { CreateID("commodity_luxury_food"), CreateID("commodity_food") };

        float timeSinceLastEaten = 5.f;
        void Update(const float delta) override
        {
            auto* ship = Utils::GetCShip();
            if (!ship)
            {
                return;
            }

            timeSinceLastEaten -= delta;
            if (timeSinceLastEaten < 0.0f)
            {
                timeSinceLastEaten = 5.0f;

                bool foundFood = false;
                for (auto traverser = ship->equipManager.begin(); traverser != ship->equipManager.end(); ++traverser)
                {
                    EquipDesc desc;
                    traverser.currentEquip->GetEquipDesc(desc);

                    if (std::ranges::find(possibleFuel, traverser.currentEquip->archetype->archId) != possibleFuel.end())
                    {
                        foundFood = true;
                        break;
                    }
                }

                if (!foundFood)
                {
                    if (ship->get_relative_health() < 0.03f)
                    {
                        pub::SpaceObj::Destroy(ship->id, DestroyType::Fuse);
                        return;
                    }

                    float relativeHealth = 0.f;
                    pub::SpaceObj::GetRelativeHealth(ship->id, relativeHealth);
                    pub::SpaceObj::SetRelativeHealth(ship->id, relativeHealth - 0.03f); // Remove 3% of their health
                }
            }
        }

    public:
        explicit DontStarve(const EffectInfo& effectInfo) : PersistentEffect(effectInfo) {}
};

// clang-format off
SetupEffect(DontStarve, {
    .effectName = "Don't Starve",
    .description = "Trent is hungry and eating a lot. If you're not careful, he'll start eating your ship.",
    .category = EffectType::Misc,
    .timingModifier = 0.0f,
    .isTimed = false
});