#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Utilities/Constants.hpp"

class SwissDiplomacy final : public ActiveEffect
{
        std::vector<std::string> ids{};
        void Begin() override
        {
            int rep;
            pub::Player::GetRep(1, rep);

            for (auto id : ids)
            {
                if (std::ranges::any_of(Constants::StoryFactions(), [&id](auto faction) { return MakeId(id.c_str()) == faction; }))
                {
                    continue;
                }

                unsigned npc;
                pub::Reputation::GetReputationGroup(npc, id.c_str());
                pub::Reputation::SetReputation(rep, npc, 0.0f);
            }
        }

    public:
        SwissDiplomacy()
        {
            INI_Reader ini;
            ini.open("../DATA/initialworld.ini", false);
            while (ini.read_header())
            {
                if (!ini.is_header("Group"))
                {
                    continue;
                }

                while (ini.read_value())
                {
                    if (!ini.is_value("nickname"))
                    {
                        continue;
                    }

                    ids.emplace_back(std::string(ini.get_value_string()));
                    break;
                }
            }
        }

        OneShotEffectInfo("Swiss Diplomacy", EffectType::Npc);
};
