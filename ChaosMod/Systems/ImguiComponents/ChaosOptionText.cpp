#include "PCH.hpp"

#include "ChaosOptionText.hpp"

#include <imgui.h>

void ChaosOptionText::ToggleVisibility() { show = !show; }

void ChaosOptionText::Render() const
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
    location.y = 0 + location.y / 6.0f;

    ImGui::SetNextWindowPos(location, ImGuiCond_Always);
    if (!ImGui::Begin("Selection Window", nullptr, windowFlags))
    {
        ImGui::End();
        return;
    }

    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("1.) Spawn Big Bertha");
    ImGui::NewLine();
    ImGui::Text("2.) Spawn Big Bertha");
    ImGui::NewLine();
    ImGui::Text("3.) Spawn Big Bertha");
    ImGui::NewLine();
    ImGui::Text("4.) Spawn Big Bertha");
    ImGui::NewLine();

    ImGui::SetWindowFontScale(1.0f);

    ImGui::End();
}
