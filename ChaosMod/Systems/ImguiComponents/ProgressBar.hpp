#pragma once

#include "Systems/ConfigManager.hpp"

#include "imgui/imgui.h"

#include "magic_enum_flags.hpp"

class ImGuiManager;
class ProgressBar final
{
        friend ImGuiManager;
        inline static float progress = 0.0f;

        enum class ValueBarFlags
        {
            Horizontal = 0,
            Vertical = 1 << 0,
        };

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

            const auto s = hms.seconds();
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

        // Similar to `ImGui::ProgressBar`, but with a horizontal/vertical switch.
        // The value text doesn't follow the value like `ImGui::ProgressBar`.
        // Here it's simply displayed in the middle of the bar.
        // Horizontal labels are placed to the right of the rect.
        // Vertical labels are placed below the rect.
        // Source: https://github.com/ocornut/imgui/issues/5263
        static void ValueBar(const float value, const ImVec2& size, const float min = 0, const float max = 1,
                             const ValueBarFlags flags = ValueBarFlags::Horizontal)
        {
            using namespace ImGui;
            using namespace magic_enum::bitwise_operators;

            const bool isHorizontal = !magic_enum::enum_flags_test(flags, ValueBarFlags::Vertical);
            const auto& style = GetStyle();
            const auto& drawList = GetWindowDrawList();
            const auto& cursorPos = GetCursorScreenPos();
            const float fraction = (value - min) / max;
            const float frameHeight = GetFrameHeight();
            const auto& rectSize = isHorizontal ? ImVec2{ CalcItemWidth(), frameHeight } : ImVec2{ GetFontSize() * 2, size.y };
            const auto& rectStart = cursorPos + ImVec2{ isHorizontal ? 0 : std::max(0.0f, rectSize.x / 2), 0 };

            drawList->AddRectFilled(rectStart, rectStart + rectSize, GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);
            drawList->AddRectFilled(rectStart + ImVec2{ 0, isHorizontal ? 0 : (1 - fraction) * rectSize.y },
                                    rectStart + rectSize * ImVec2{ isHorizontal ? fraction : 1, 1 },
                                    GetColorU32(ImGuiCol_PlotHistogram),
                                    style.FrameRounding,
                                    isHorizontal ? ImDrawFlags_RoundCornersLeft : ImDrawFlags_RoundCornersBottom);
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
            ImGui::SetNextWindowPos({ -40.f, -10.f }, ImGuiCond_Always);
            ImGui::SetNextWindowSize(viewport->Size + ImVec2{ 60.f, 10.f }, ImGuiCond_Always);
            if (ImGui::Begin("ProgressWindow",
                             nullptr,
                             ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_NoTitleBar))
            {
                ValueBar(progress, ImVec2(100, viewport->Size.y), 0.0f, 1.0f, ValueBarFlags::Vertical);

                const auto vec = ImGui::GetWindowSize();
                ImGui::SetCursorScreenPos(vec - ImVec2{ 140.f, vec.y });
                ValueBar(progress, ImVec2(100.f, viewport->Size.y), 0.0f, 1.0f, ValueBarFlags::Vertical);
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
