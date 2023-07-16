#include "PCH.hpp"

#include "ProgressBar.hpp"

#include <imgui.h>

void ProgressBar::SetProgressPercentage(const float percent) { progress = percent; }

void ProgressBar::Render() const
{
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetColorU32(0x000000FF));
    if (ImGui::BeginMainMenuBar())
    {
        // TODO: Customizable colour for chaos bar
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::GetColorU32(0xFFFFFFFF));
        ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), "");
        ImGui::PopStyleColor();
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleColor();
}
