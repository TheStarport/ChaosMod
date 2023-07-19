#include "PCH.hpp"

#include "ProgressBar.hpp"

#include "Systems/ConfigManager.hpp"

#include <imgui.h>

void ProgressBar::SetProgressPercentage(const float percent) { progress = percent; }

void ProgressBar::Render() const
{
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetColorU32(0x000000FF));
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ConfigManager::i()->progressBarColor);
        ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), "");
        ImGui::PopStyleColor();
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleColor();
}
