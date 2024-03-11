#pragma once

#include "../ImGuiManager.hpp"

#include <imgui/imgui.h>

class ImGuiManager;
class DebugMenu final
{
        friend ImGuiManager;

        inline static ImGuiTextBuffer buf;
        inline static ImGuiTextFilter filter;
        inline static ImVector<int> lineOffsets;

        inline static bool show =
#ifdef _DEBUG
            true;
#else
            false;
#endif

        static void Render()
        {
            if (!show)
            {
                return;
            }

            ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Debug Console", &show))
            {
                ImGui::End();
                return;
            }

            ImGui::SameLine();
            const bool clear = ImGui::Button("Clear");
            ImGui::SameLine();
            const bool copy = ImGui::Button("Copy");
            ImGui::SameLine();
            filter.Draw("Filter", -100.0f);

            if (ImGui::Button("Open Configurator"))
            {
                ImGuiManager::ShowConfigurator();
            }

            if (ImGui::Button("Open Debug Event Selector"))
            {
                ImGuiManager::ShowEffectSelector();
            }

            ImGui::Separator();

            if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
            {
                if (clear)
                {
                    buf.clear();
                    lineOffsets.clear();
                    lineOffsets.push_back(0);
                }
                if (copy)
                {
                    ImGui::LogToClipboard();
                }

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                const char* b = buf.begin();
                const char* bend = buf.end();
                if (filter.IsActive())
                {
                    // In this example we don't use the clipper when Filter is enabled.
                    // This is because we don't have random access to the result of our filter.
                    // A real application processing logs with ten of thousands of entries may want to store the result of
                    // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                    for (int lineNo = 0; lineNo < lineOffsets.Size; lineNo++)
                    {
                        const char* lineStart = b + lineOffsets[lineNo];
                        if (const char* lineEnd = (lineNo + 1 < lineOffsets.Size) ? (b + lineOffsets[lineNo + 1] - 1) : bend;
                            filter.PassFilter(lineStart, lineEnd))
                        {
                            ImGui::TextUnformatted(lineStart, lineEnd);
                        }
                    }
                }
                else
                {
                    ImGuiListClipper clipper;
                    clipper.Begin(lineOffsets.Size);
                    while (clipper.Step())
                    {
                        for (int lineNo = clipper.DisplayStart; lineNo < clipper.DisplayEnd; lineNo++)
                        {
                            const char* lineStart = b + lineOffsets[lineNo];
                            const char* lineEnd = (lineNo + 1 < lineOffsets.Size) ? (b + lineOffsets[lineNo + 1] - 1) : bend;
                            ImGui::TextUnformatted(lineStart, lineEnd);
                        }
                    }
                    clipper.End();
                }
                ImGui::PopStyleVar();

                // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
                // Using a scrollbar or mouse-wheel will take away from the bottom edge.
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
            ImGui::End();
        }

    public:
        DebugMenu() = delete;

        static void Log(std::string log)
        {
            log += "\n";
            int oldSize = buf.size();
            buf.append(log.c_str(), log.c_str() + log.size());
            for (const int newSize = buf.size(); oldSize < newSize; oldSize++)
            {
                if (buf[oldSize] == '\n')
                {
                    lineOffsets.push_back(oldSize + 1);
                }
            }
        }
};
