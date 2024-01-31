#pragma once

#include "Systems/PatchNotes/PatchNotes.hpp"
#include "imgui/imgui.h"

class ImGuiManager;
class PatchNotesWindow final
{
        friend ImGuiManager;

        inline static bool show = false;

        inline static DWORD* gamePauseIncrementor = PDWORD(0x667D54);
        inline static bool doDecrement = false;
        static void Render()
        {
            if (!show)
            {
                if (doDecrement)
                {
                    *gamePauseIncrementor = 0;
                    doDecrement = false;
                }
                return;
            }

            if (!*gamePauseIncrementor)
            {
                *gamePauseIncrementor = 1;
                doDecrement = true;
            }

            ImGui::SetNextWindowSize({ 1280.f, 1024.f }, ImGuiCond_Always);
            const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if (!ImGui::Begin("Patch Windows", &show, ImGuiWindowFlags_NoResize))
            {
                ImGui::End();
                return;
            }

            static auto& notes = PatchNotes::GetPatchNotes();
            for (auto patch = notes.rbegin(); patch != notes.rend(); ++patch)
            {
                ImGui::SeparatorText(patch->version.c_str());
                ImGui::Text(patch->date.c_str()); // NOLINT(clang-diagnostic-format-security)
                if (!patch->releaseName.empty())
                {
                    ImGui::Text(patch->releaseName.c_str()); // NOLINT(clang-diagnostic-format-security)
                }
                ImGui::NewLine();

                for (auto& text : patch->changes)
                {
                    ImGui::Text(text.c_str()); // NOLINT(clang-diagnostic-format-security)
                }
            }

            ImGui::End();
        }

    public:
        PatchNotesWindow() = delete;
};
