#pragma once

#include "Systems/PatchNotes/PatchNotes.hpp"
#include "imgui/imgui.h"

class ImGuiManager;
class PatchNotesWindow final
{
        friend ImGuiManager;

        inline static bool show = false;
        inline static char patchFilter[255];

        inline static DWORD* gamePauseIncrementor = PDWORD(0x667D54);
        inline static bool doDecrement = false;

        static void ToLower(char* s)
        {
            for (char* p = s; *p; p++)
            {
                *p = tolower(*p);
            }
        }

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

            ImGui::InputText("Filter", patchFilter, sizeof(patchFilter));
            ToLower(patchFilter);

            static auto& notes = PatchNotes::GetPatchNotes();
            for (auto patch = notes.rbegin(); patch != notes.rend(); ++patch)
            {
                // If there is a filter, check if this patch contains ANY items we care about first
                if (patchFilter[0] != '\0' && std::ranges::all_of(patch->changes,
                                                                  [](const std::string& text)
                                                                  {
                                                                      char* lowerA = _strdup(text.c_str());
                                                                      ToLower(lowerA);

                                                                      const bool ret = strstr(lowerA, patchFilter) == nullptr;

                                                                      free(lowerA);

                                                                      return ret;
                                                                  }))
                {
                    continue;
                }

                ImGui::SeparatorText(patch->version.c_str());
                ImGui::Text(patch->date.c_str()); // NOLINT(clang-diagnostic-format-security)
                if (!patch->releaseName.empty())
                {
                    ImGui::Text(patch->releaseName.c_str()); // NOLINT(clang-diagnostic-format-security)
                }
                ImGui::NewLine();

                for (auto& text : patch->changes)
                {
                    char* lowerA = _strdup(text.c_str());
                    ToLower(lowerA);

                    if (patchFilter[0] == '\0' || strstr(lowerA, patchFilter) != nullptr)
                    {
                        ImGui::Text(text.c_str()); // NOLINT(clang-diagnostic-format-security)
                    }
                    free(lowerA);
                }
            }

            ImGui::End();
        }

    public:
        PatchNotesWindow() = delete;
};
