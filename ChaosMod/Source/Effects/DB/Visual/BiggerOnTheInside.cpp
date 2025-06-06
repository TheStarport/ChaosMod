#pragma once
#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class BiggerOnTheInside final : public ActiveEffect
{
        std::string originalStarSphere;

        // clang-format off
        std::array<const char*, 8> backgrounds = {
            R"(BASES\LIBERTY\li_battleship_deck.cmp)",
            R"(BASES\LIBERTY\li_depot_deck.cmp)",
            R"(BASES\RHEINLAND\rh_outpost_deck.cmp)",
            R"(BASES\RHEINLAND\rh_depot_deck.cmp)",
            R"(BASES\BRETONIA\br_outpost_deck.cmp)",
            R"(BASES\BRETONIA\br_depot_deck.cmp)",
            R"(BASES\KUSARI\ku_outpost_deck.cmp)",
            R"(BASES\KUSARI\ku_sanctum.cmp)",
        };
        // clang-format on

        void ChangeBackground(const bool reset) const
        {
            const auto index = Get<Random>()->Uniform(0u, backgrounds.size() - 1);
            Utils::SetNewStarSphere(reset ? originalStarSphere.c_str() : backgrounds[index]);
        }

        void Begin() override
        {
            originalStarSphere = Utils::GetCurrentStarSphere();

            ChangeBackground(false);
        }

        void End() override { ChangeBackground(true); }

    public:
        explicit BiggerOnTheInside(const EffectInfo& effectInfo) : ActiveEffect(effectInfo) {}
};

// clang-format off
SetupEffect(BiggerOnTheInside, {
    .effectName = "Bigger On The Inside",
    .description = "Ever wondered what it would be like to fly around a hangar?",
    .category = EffectType::Visual,
    .exclusion = EffectExclusion::StarSphere
});