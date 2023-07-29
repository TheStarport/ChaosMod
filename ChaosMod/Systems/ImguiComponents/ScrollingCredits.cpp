#include "PCH.hpp"

#include "ScrollingCredits.hpp"

#include "Systems/ChaosTimer.hpp"
#include "Systems/ConfigManager.hpp"
#include "Systems/UiManager.hpp"

#include <imgui.h>

ScrollingCredits::ScrollingCredits()
{
    const std::string allLines =
#include "Credits.txt"
        ;
    credits = Utils::String::Split(allLines, '\n');
}

void ScrollingCredits::ToggleVisibility()
{
    show = !show;
    scrollAmount = 0.0f;
}

void ScrollingCredits::Render()
{
    const auto newTime = std::chrono::high_resolution_clock::now().time_since_epoch();
    std::chrono::duration<float> frameTime = newTime - currentTime;
    currentTime = newTime;

    if (!show)
    {
        return;
    }

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus |
                                             ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();
    if (!ImGui::Begin("Scrolling Credits", nullptr, windowFlags))
    {
        ImGui::End();
        return;
    }

    ImGui::BeginChild("sub-credits", {}, false, windowFlags);

    // Add padding
    ImGui::Dummy({ 0.0f, ImGui::GetWindowHeight() });

    for (auto& line : credits)
    {
        ImGui::PushFont(std::ranges::any_of(line, IsCharLowerA) ? UiManager::GetFont(UiManager::Font::TitiliumWeb)
                                                                : UiManager::GetFont(UiManager::Font::TitiliumWebBoldLarge));
        ImGuiStyle& style = ImGui::GetStyle();

        float size = ImGui::CalcTextSize(line.c_str()).x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - size) * 0.5f;
        if (off > 0.0f)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }

        ImGui::Text(line.c_str());
        ImGui::PopFont();
    }

    // Add padding
    ImGui::Dummy({ 0.0f, ImGui::GetWindowHeight() });

    ImGui::SetScrollY(scrollAmount);
    const float max = ImGui::GetScrollMaxY();

    if (max >= scrollAmount)
    {
        scrollAmount += frameTime.count() * 20;
    }
    else if (max != 0.0f)
    {
        show = false;
        scrollAmount = 0.0f;
    }

    ImGui::EndChild();
    ImGui::End();
}
