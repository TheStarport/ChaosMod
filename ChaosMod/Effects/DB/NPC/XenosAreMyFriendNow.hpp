#pragma once
#include "Effects/ActiveEffect.hpp"

class XenosAreMyFriendsNow final : public ActiveEffect
{
        std::vector<std::string> ids{};
        void Begin() override
        {
            int rep;
            pub::Player::GetRep(1, rep);

            for (auto id : ids)
            {
                unsigned npc;
                pub::Reputation::GetReputationGroup(npc, id.c_str());
                pub::Reputation::SetReputation(rep, npc, -0.9f);
            }

            unsigned npc;
            pub::Reputation::GetReputationGroup(npc, "fc_x_grp");
            pub::Reputation::SetReputation(rep, npc, 1.0f);
        }

    public:
        XenosAreMyFriendsNow()
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
                    if (!ini.is_value("nickname") || std::string(ini.get_value_string()) == "fc_x_grp")
                    {
                        continue;
                    }

                    ids.emplace_back(std::string(ini.get_value_string()));
                    break;
                }
            }
        }

        DefEffectInfo("Xenos Are My Friends Now", 0.0f, EffectType::Npc);
};