#pragma once

#include "Systems/ConfigManager.hpp"
#include "imgui/imgui.h"

class ImGuiManager;
class ProgressBar final
{
        friend ImGuiManager;
        inline static float progress = 0.0f;

        static void Render()
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

    public:
        ProgressBar() = delete;
};
