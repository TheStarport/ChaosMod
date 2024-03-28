#pragma once

#include "CoreComponents/PatchNotes.hpp"
#include "imgui.h"

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

            static auto patchType = Change::ChangePositivity::Default;

            ImGui::RadioButton("All", reinterpret_cast<int*>(&patchType), 0);
            ImGui::SameLine();
            ImGui::RadioButton("Boon", reinterpret_cast<int*>(&patchType), 1);
            ImGui::SameLine();
            ImGui::RadioButton("Nerf", reinterpret_cast<int*>(&patchType), 2);
            ImGui::SameLine();
            ImGui::RadioButton("Neither", reinterpret_cast<int*>(&patchType), 3);

            ImGui::Separator();

            static auto& notes = PatchNotes::GetPatchNotes();
            for (auto patch = notes.rbegin(); patch != notes.rend(); ++patch)
            {
                // If there is a filter, check if this patch contains ANY items we care about first
                if (patchFilter[0] != '\0' && std::ranges::all_of(patch->changes,
                                                                  [](const std::pair<std::string, Change::ChangePositivity>& change)
                                                                  {
                                                                      char* lowerA = _strdup(change.first.c_str());
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

                for (auto& change : patch->changes)
                {
                    if (patchType != Change::ChangePositivity::Default && patchType != change.second)
                    {
                        continue;
                    }

                    char* lowerA = _strdup(change.first.c_str());
                    ToLower(lowerA);

                    if (patchFilter[0] == '\0' || strstr(lowerA, patchFilter) != nullptr)
                    {
                        if (Get<ConfigManager>()->patchNotes.displayInColor)
                        {
                            ImVec4 col = { 1.f, 1.f, 1.f, 1.f };
                            if (change.first[0] == '+')
                            {
                                col = { 0.30f, 1.00f, 0.30f, 1.0f };
                            }
                            else if (change.first[0] == '-')
                            {
                                col = { 1.f, 0.15f, 0.f, 1.f };
                            }
                            else if (change.first[0] == '~')
                            {
                                col = { 1.f, 0.85f, 0.f, 1.f };
                            }

                            ImGui::TextColored(col, change.first.c_str());
                        }
                        else
                        {
                            ImGui::Text(change.first.c_str()); // NOLINT(clang-diagnostic-format-security)
                        }
                    }
                    free(lowerA);
                }
            }

            ImGui::End();
        }

    public:
        PatchNotesWindow() = delete;
};
