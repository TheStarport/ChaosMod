#pragma once
#include "CoreComponents/TwitchVoting.hpp"

class ImGuiManager;
class ChaosOptionText final
{
        friend ImGuiManager;

        inline static std::vector<std::string> options;

        static void Render()
        {
            if (options.empty() || !Utils::GetCShip())
            {
                return;
            }

            constexpr ImGuiWindowFlags windowFlags =
                ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

            ImGui::SetNextWindowSize(ImVec2(500, 420), ImGuiCond_Always);

            auto location = ImGui::GetIO().DisplaySize;
            location.x = 0 + location.x / 10.0f;
            location.y = 0 + location.y / 6.0f;

            ImGui::SetNextWindowPos(location, ImGuiCond_Always);
            if (!ImGui::Begin("Selection Window", nullptr, windowFlags))
            {
                ImGui::End();
                return;
            }

            const auto config = Get<ConfigManager>();
            const auto& [totalVotes, votes, votePercentages] = Get<TwitchVoting>()->GetCurrentVoteInfo();

            for (int i = 0; i < options.size(); ++i)
            {
                ImGui::Text(options[i].c_str()); // NOLINT(clang-diagnostic-format-security)

                if (i < votes.size())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0, 255, 0).Value);

                    if (config->chaosSettings.twitchVoteVisibility == ChaosSettings::TwitchVoteVisibility::PerEffectTotal)
                    {
                        ImGui::SameLine();
                        ImGui::Text(std::format("{} / {}", votes[i], totalVotes).c_str());
                    }
                    else if (config->chaosSettings.twitchVoteVisibility == ChaosSettings::TwitchVoteVisibility::PerEffectPercentage)
                    {
                        ImGui::SameLine();
                        ImGui::Text(std::format("{}%%", static_cast<int>(votePercentages[i] * 100)).c_str());
                    }

                    ImGui::PopStyleColor();
                }

                ImGui::NewLine();
            }

            if (config->chaosSettings.twitchVoteVisibility != ChaosSettings::TwitchVoteVisibility::NotVisible)
            {
                ImGui::Text(std::format("Total Votes: {}", totalVotes).c_str());
            }

            ImGui::End();
        }

    public:
        ChaosOptionText() = delete;
};
