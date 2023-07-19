#include "PCH.hpp"

#include "ActiveEffectsText.hpp"

#include "Systems/ChaosTimer.hpp"
#include "Systems/ConfigManager.hpp"
#include "Systems/UiManager.hpp"

#include <imgui.h>

void ActiveEffectsText::ToggleVisibility() { show = !show; }

void ActiveEffectsText::Render() const
{
    if (!show)
    {
        return;
    }

    constexpr ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Always);

    auto location = ImGui::GetIO().DisplaySize;
    location.x = location.x - location.x / 4.0f;
    location.y = 0 + location.y / 2.0f;

    ImGui::SetNextWindowPos(location, ImGuiCond_Always);
    if (!ImGui::Begin("Active Effects Window", nullptr, windowFlags))
    {
        ImGui::End();
        return;
    }

    for (const auto& effects = ChaosTimer::GetActiveEffects(); auto& effect : effects)
    {
        auto info = effect.first->GetEffectInfo();
        ImGui::Text(info.effectName.c_str()); // NOLINT

        if (info.isTimed)
        {
            const float fraction = effect.second / (info.timingModifier * ConfigManager::i()->defaultEffectDuration);

            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 200);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ConfigManager::i()->progressBarColor);
            ImGui::ProgressBar(fraction, ImVec2(200, 0), "");
            ImGui::PopStyleColor();
        }

        ImGui::NewLine();
    }

    ImGui::End();
}
