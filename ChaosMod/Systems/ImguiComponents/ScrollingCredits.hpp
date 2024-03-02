#pragma once

#include "ImGuiManager.hpp"
#include "imgui/imgui.h"

class ScrollingCredits final
{
        friend ImGuiManager;

        inline static bool show = false;
        inline static float scrollAmount = 0.0f;
        inline static std::chrono::duration<long long, std::ratio<1, 1000000000>> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
        inline static std::vector<std::string> credits;

        static void Init()
        {
            const std::string allLines =
#include "Credits.txt"
                ;

            for (auto line : StringUtils::GetParams(allLines, '\n'))
            {
                credits.emplace_back(line);
            }
        }

        static void Render()
        {
            const auto newTime = std::chrono::high_resolution_clock::now().time_since_epoch();
            const std::chrono::duration<float> frameTime = newTime - currentTime;
            currentTime = newTime;

            if (!show)
            {
                return;
            }

            constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus |
                                                     ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar |
                                                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus;

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
                ImGui::PushFont(std::ranges::any_of(line, IsCharLowerA) ? ImGuiManager::GetFont(Font::TitiliumWeb)
                                                                        : ImGuiManager::GetFont(Font::TitiliumWebBoldLarge));
                const ImGuiStyle& style = ImGui::GetStyle();

                const float size = ImGui::CalcTextSize(line.c_str()).x + style.FramePadding.x * 2.0f;
                const float avail = ImGui::GetContentRegionAvail().x;

                if (const float off = (avail - size) * 0.5f; off > 0.0f)
                {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
                }

                ImGui::Text(line.c_str()); // NOLINT(clang-diagnostic-format-security)
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
};
