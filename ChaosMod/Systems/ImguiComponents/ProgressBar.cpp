#include "PCH.hpp"

#include "ProgressBar.hpp"

#include "Systems/ConfigManager.hpp"

#include <imgui/imgui.h>

void ProgressBar::SetProgressPercentage(const float percent) { progress = percent; }

void ProgressBar::Render() const
{
    ImGui::PushStyleColor(ImGuiCol_Text, 0x0);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, 0x0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x0);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(::ImGuiCol_FrameBg, 0.7f));
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ConfigManager::i()->progressBarColor);
        ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), "");
        ImGui::PopStyleColor();
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleColor(4);
}
