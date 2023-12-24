#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"

class Screaming final : public ActiveEffect
{
        float interval = 0.0f;

        std::vector<std::pair<uint, uint>> voiceToLinePairs;

        std::vector<CShip*> activeShips;

        void Update(float delta) override
        {
            interval -= delta;
            if (interval <= 0.0f)
            {
                interval = 1.0f;

                activeShips.clear();

                Utils::ForEachObject<CShip>(CObject::Class::CSHIP_OBJECT,
                                            [this](CShip* ship)
                                            {
                                                if (ship->is_player())
                                                {
                                                    return;
                                                }

                                                activeShips.emplace_back(ship);
                                            });

                if (!activeShips.empty())
                {
                    const auto ship = activeShips[Random::i()->Uniform(0u, activeShips.size() - 1)];
                    const uint index = Random::i()->Uniform(0u, voiceToLinePairs.size() - 1);
                    const auto& [voice, line] = voiceToLinePairs[index];

                    ID_String voiceLine{};
                    voiceLine.id = line;
                    pub::SpaceObj::SendComm(ship->id, ship->id, voice, nullptr, 0, reinterpret_cast<uint*>(&voiceLine), 9, 19009, 1.0f, true);
                }
            }
        }

    public:
        explicit Screaming(const EffectInfo& effectInfo) : ActiveEffect(effectInfo)
        {
            const uint female01 = CreateID("atc_leg_f01");
            const uint male01 = CreateID("pilot_f_mil_m01");
            const uint male02 = CreateID("pilot_f_mil_m02");
            voiceToLinePairs = {
                {female01, 0xBDEC9B8E},
                {female01, 0xBCC89B04},
                {female01, 0xBCA49B0E},
                {female01, 0xBCA09B04},
                {female01, 0xBC589B44},
                {female01, 0xBC349B4E},
                {female01, 0xBC309B44},
                {female01, 0xB3472781},
                {female01, 0xB343278B},
                {female01, 0xB32B278B},
                {  male01, 0xBCC89B04},
                {  male01, 0xBCA49B0E},
                {  male01, 0xBCA09B04},
                {  male01, 0xBC79874A},
                {  male01, 0xBC5C9B4E},
                {  male01, 0xBC589B44},
                {  male02, 0xBDEC9B8E},
                {  male02, 0xBCA49B0E},
                {  male02, 0xBC349B4E},
                {  male02, 0xBC309B44},
                {  male02, 0xBC5C9B4E},
                {  male02, 0xBC589B44},
            };
        }
};

// clang-format off
SetupEffect(Screaming, {
    .effectName = "SCREAMING",
    .description = "Every NPC - they scream and you scream at their screaming and everyone is screaming",
    .category = EffectType::Npc
});