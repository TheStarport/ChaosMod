#include "PCH.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Constants.hpp"

class XenosAreMyFriendsNow final : public ActiveEffect
{
        std::vector<std::string> ids{};
        void Begin() override
        {
            int rep;
            pub::Player::GetRep(1, rep);

            for (auto& id : ids)
            {
                // Ignore story factions
                if (std::ranges::any_of(Constants::StoryFactions(), [&id](auto faction) { return MakeId(id.c_str()) == faction; }))
                {
                    continue;
                }

                unsigned npc;
                pub::Reputation::GetReputationGroup(npc, id.c_str());
                pub::Reputation::SetReputation(rep, npc, -0.9f);
            }

            unsigned npc;
            pub::Reputation::GetReputationGroup(npc, "fc_x_grp");
            pub::Reputation::SetReputation(rep, npc, 1.0f);
        }

    public:
        explicit XenosAreMyFriendsNow(const EffectInfo& info) : ActiveEffect(info)
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
};

// clang-format off
SetupEffect(XenosAreMyFriendsNow, {
    .effectName = "Xenos are my friends now",
    .description = "Friendship ended with everyone else, Xenos are my friends now. Your rep sheet might look a bit on the red side until you fix it...",
    .category = EffectType::Npc,
    .timingModifier = 0.0f,
    .isTimed = false
});
