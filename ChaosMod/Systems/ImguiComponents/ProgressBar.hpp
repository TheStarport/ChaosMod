#pragma once

#include "Systems/ConfigManager.hpp"

#include "imgui/imgui.h"

#include "magic_enum_flags.hpp"

class ImGuiManager;
class ProgressBar final
{
        friend ImGuiManager;
        inline static float progress = 0.0f;

        static void DrawAngledLine(const float angle, const float radius, const float lengthIn, const float lengthOut, const ImU32 color)
        {
            const float x1 = std::sinf(-angle + static_cast<float>(std::numbers::pi)) * (radius - lengthOut) + radius;
            const float y1 = std::cosf(-angle + static_cast<float>(std::numbers::pi)) * (radius - lengthOut) + radius;

            const float x2 = std::sinf(-angle + static_cast<float>(std::numbers::pi)) * (radius - lengthIn) + radius;
            const float y2 = std::cosf(-angle + static_cast<float>(std::numbers::pi)) * (radius - lengthIn) + radius;

            ImDrawList* draw = ImGui::GetForegroundDrawList();
            draw->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, 3.f);
        }

        static void DrawClock()
        {
            const auto tp = std::chrono::system_clock::now();
            const auto dp = floor<std::chrono::days>(tp);

            const std::chrono::hh_mm_ss hms{ floor<std::chrono::milliseconds>(tp - dp) };

            const auto ms = hms.subseconds();

            // Get the draw buffer
            ImDrawList* draw = ImGui::GetForegroundDrawList();
            const ImU32 color = ImGui::GetColorU32(ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 1.f));

            constexpr float outerRadius = 200.f;
            constexpr float innerRadius = 5.f;
            constexpr ImVec2 center(outerRadius, outerRadius);

            // Draw the clock itself
            draw->AddCircle(center, outerRadius, color, 0, 3.f);
            draw->AddCircle(center, innerRadius, color, 0, 3.f);

            for (int i = 0; i < 12; ++i)
            {
                const float angle = (i / 12.f) * 2 * static_cast<float>(std::numbers::pi);
                constexpr float lengthIn = 20.f;

                DrawAngledLine(angle, outerRadius, lengthIn, 0.f, color);
            }

            // Draw the clock arms
            const ImU32 colorArm = ImGui::GetColorU32(ImVec4(252.f / 255.f, 120.f / 255.f, 120.f / 255.f, 1.f));
            const ImU32 colorMs = ImGui::GetColorU32(ImVec4(120.f / 255.f, 120.f / 255.f, 240.f / 255.f, 1.f));

            constexpr float msMax = 1000.f;
            const auto msRatio = static_cast<float>(ms.count()) / msMax;

            DrawAngledLine(progress * 2.f * static_cast<float>(std::numbers::pi), outerRadius, outerRadius - innerRadius, 20.f, colorArm);
            DrawAngledLine(msRatio * 2.f * static_cast<float>(std::numbers::pi), outerRadius, 30.f, 0.f, colorMs);
        }

        static void VerticalBar(const float value, const ImVec2& size)
        {
            using namespace ImGui;
            using namespace magic_enum::bitwise_operators;

            const auto& style = GetStyle();
            const auto& drawList = GetWindowDrawList();

            const auto& cursorPos = GetCursorScreenPos();
            const float fraction = value / 1.f;

            drawList->AddRectFilled(cursorPos, cursorPos + size, GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);
            drawList->AddRectFilled(cursorPos + ImVec2{ 0, (1 - fraction) * size.y },
                                    cursorPos + size * ImVec2{ 1, 1 },
                                    GetColorU32(ImGuiCol_PlotHistogram),
                                    style.FrameRounding,
                                    ImDrawFlags_RoundCornersBottom);
        }

        static void DrawTopBar()
        {
            if (ImGui::BeginMainMenuBar())
            {
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ConfigManager::i()->progressBarColor);
                ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), "");
                ImGui::PopStyleColor();
                ImGui::EndMainMenuBar();
            }
        }

        static void DrawSideBars()
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            // For reasons, I cannot understand, we need to offset the window by a few pixels
            ImGui::SetNextWindowPos({ -20.f, 0.0f }, ImGuiCond_Always);
            ImGui::SetNextWindowSize(viewport->Size + ImVec2{ 30.f, 10.f }, ImGuiCond_Always);
            if (ImGui::Begin("ProgressWindow",
                             nullptr,
                             ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_NoTitleBar))
            {
                ImGui::SetCursorScreenPos({ 0.f, 0.f });
                VerticalBar(progress, ImVec2(100.f, viewport->Size.y));

                const auto vec = ImGui::GetWindowSize();
                ImGui::SetCursorScreenPos(vec - ImVec2{ 120.f, vec.y });
                VerticalBar(progress, ImVec2(100.f, viewport->Size.y));
            }

            ImGui::End();
        }

        static void Render()
        {
            // Do not render on bases
            if (!Utils::GetCShip())
            {
                return;
            }

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ConfigManager::i()->progressBarColor);

            switch (ConfigManager::i()->progressBarType)
            {
                case ConfigManager::ProgressBar::TopBar: DrawTopBar(); break;
                case ConfigManager::ProgressBar::SideBar: DrawSideBars(); break;
                case ConfigManager::ProgressBar::Clock: DrawClock(); break;
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }

    public:
        ProgressBar() = delete;
};
