#pragma once

#include "Systems/ChaosTimer.hpp"
#include "Systems/ConfigManager.hpp"
#include "imgui/imgui.h"

class ImGuiManager;
class ActiveEffectsText final
{
        friend ImGuiManager;

        inline static bool boxOfChocolates;
        inline static bool show = true;

        static void WriteEffect(const std::string& name, const bool isTimed, const float time, const float modifier)
        {
            ImGui::Text(name.c_str()); // NOLINT

            if (isTimed)
            {
                const float fraction = time / (modifier * Get<ConfigManager>()->defaultEffectDuration);

                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 200);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Get<ConfigManager>()->progressBarColor);
                ImGui::PushStyleColor(ImGuiCol_Text, Get<ConfigManager>()->progressBarTextColor);

                const std::string timeRemaining = Get<ConfigManager>()->showTimeRemainingOnEffects ? std::format("{:.1f}", time) : "";
                ImGui::ProgressBar(fraction, ImVec2(200, 0), timeRemaining.c_str());
                ImGui::PopStyleColor(2);
            }

            ImGui::NewLine();
        }

        static void Render()
        {
            if (!show)
            {
                return;
            }

            constexpr ImGuiWindowFlags windowFlags =
                ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

            auto location = ImGui::GetIO().DisplaySize;
            location.x = location.x - location.x / 4.0f;

            ImGui::SetNextWindowSize(ImVec2(500, location.y - (location.y * 10 / 100)), ImGuiCond_Always);

            location.y = 200;
            ImGui::SetNextWindowPos(location, ImGuiCond_Always);
            if (!ImGui::Begin("Active Effects Window", nullptr, windowFlags))
            {
                ImGui::End();
                return;
            }

            if (boxOfChocolates)
            {
                auto effects = ChaosTimer::GetActiveEffects();
                if (const auto effect = std::ranges::find_if(effects, [](auto& ef) { return ef.first->GetEffectInfo().effectName == "Box Of Chocolates"; });
                    effect == effects.end())
                {
                    Log("Box of chocolates was active, but no effect was found.");
                    boxOfChocolates = false;
                }
                else
                {
                    const auto& info = effect->first->GetEffectInfo();
                    WriteEffect(info.effectName, info.isTimed, effect->second, info.timingModifier);
                    ImGui::End();
                    return;
                }
            }

            for (const auto& effects = ChaosTimer::GetActiveEffects(); auto& effect : effects)
            {
                const auto& info = effect.first->GetEffectInfo();
                WriteEffect(info.effectName, info.isTimed, effect.second, info.timingModifier);
            }

            ImGui::End();
        }

    public:
        ActiveEffectsText() = delete;
};
