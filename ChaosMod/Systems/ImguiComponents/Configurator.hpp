#pragma once

#include "ImGuiManager.hpp"
#include "Systems/ConfigManager.hpp"
#include "Systems/PatchNotes/PatchNotes.hpp"
#include "Systems/SystemComponents/TwitchVoting.hpp"
#include "imgui/imgui.h"

class ImGuiManager;
class Configurator final
{
        friend ImGuiManager;

        inline static bool show = false;
        static void Render()
        {
            if (!show)
            {
                return;
            }

            if (!ImGui::Begin("Configurator", &show, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
            {
                ImGui::End();
                return;
            }

            auto* config = ConfigManager::i();
            if (ImGui::Button("Save Changes"))
            {
                config->Save();
            }

            ImGui::Separator();

            // Effect Settings
            ImGui::SliderFloat("Time Between Chaos", &config->timeBetweenChaos, 5.0f, 120.0f);
            ImGui::SliderFloat("Default Effect Duration", &config->defaultEffectDuration, 20.0f, 300.0f);
            ImGui::SliderInt("Total Allowed Concurrent Effects", reinterpret_cast<PINT>(&config->totalAllowedConcurrentEffects), 1, 12);
            ImGui::SliderInt("Time Between Auto Saves", reinterpret_cast<PINT>(&config->timeBetweenSavesInSeconds), 60 * 3, 60 * 10);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "Chaos Mod, by its nature, is unstable. The game might crash or soft lock you. It might set you to be hostile to everyone, "
                    "or remove all your money.\n"
                    "For this reason, the game will auto save (even in space) frequently to give you plenty of places to return to during gameplay.");
            }

            ImGui::SameLine();
            ImGui::Checkbox("Allow Saving During Combat", &config->allowAutoSavesDuringCombat);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "By default, the game will not autosave when hostiles are within 2k of the player. If they are it will push the timer back by 30 seconds.\n"
                    "This is done because this can break mission scripts and NPC spawns in some cases. You can enable it if you want, but you've been warned!");
            }

            ImGui::Checkbox("Block Teleports During Missions", &config->blockTeleportsDuringMissions);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Many missions can softlock if the player is teleports away, into a different system, or to a base. If true, this will "
                                  "prevent teleport effects from being selected while on a mission.");
            }

            ImGui::Separator();

            // Twitch Settings
            ImGui::Text("Twitch Settings");

            ImGui::Checkbox("Enable Twitch Voting", &config->enableTwitchVoting);
            ImGui::SameLine();
            ImGui::SliderFloat("Twitch Voting Weight", &config->baseTwitchVoteWeight, 0.1f, 1.0f);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "When using Twitch votes, the item with the most votes will have this percentage chance of being selected.\n This value will determine "
                    "the percentage chance of the Twitch value being selected. If set to 1, Twitch chat's option is guaranteed.\n This option is likely to "
                    "be more preferable if you have fewer engaged viewers as to still allow for a degree of randomness.");
            }

            static bool initialized = false;
            ImGui::BeginDisabled(!config->enableTwitchVoting || initialized);
            if (ImGui::Button("Initialize Voting Proxy") && TwitchVoting::i()->Initialize())
            {
                initialized = true;
            }
            ImGui::EndDisabled();

            ImGui::Separator();

            // Style Settings
            ImGui::Text("Style Settings");

            static DWORD startingProgressBarColor = config->progressBarColor;
            static auto startingProgressBarColorVec = ImGui::ColorConvertU32ToFloat4(startingProgressBarColor);
            static float progressColor[3] = { startingProgressBarColorVec.x, startingProgressBarColorVec.y, startingProgressBarColorVec.z };
            ImGui::ColorEdit3("Chaos Progress Color", progressColor, ImGuiColorEditFlags_PickerHueBar);
            config->progressBarColor = ImGui::ColorConvertFloat4ToU32(ImVec4(progressColor[0], progressColor[1], progressColor[2], 0xFF));

            static DWORD startingProgressBarTextColor = config->progressBarTextColor;
            static auto startingProgressBarTextColorVec = ImGui::ColorConvertU32ToFloat4(startingProgressBarTextColor);
            static float progressTextColor[3] = { startingProgressBarTextColorVec.x, startingProgressBarTextColorVec.y, startingProgressBarTextColorVec.z };
            ImGui::ColorEdit3("Chaos Progress Text Color", progressTextColor, ImGuiColorEditFlags_PickerHueBar);
            config->progressBarTextColor = ImGui::ColorConvertFloat4ToU32(ImVec4(progressTextColor[0], progressTextColor[1], progressTextColor[2], 0xFF));

            const char* timerOptions[] = {
                "Progress Bar: Top",
                "Progress Bar: Sides",
                "Clock",
            };

            ImGui::Combo("Chaos Progress Bar Type:", reinterpret_cast<int*>(&config->progressBarType), timerOptions, IM_ARRAYSIZE(timerOptions));

            ImGui::Checkbox("Show Time Remaining On Effects", &config->showTimeRemainingOnEffects);

            ImGui::Separator();

            ImGui::Checkbox("Enable Patch Notes", &config->enablePatchNotes);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "'Patch Notes' is a feature where the game will frequently write updates to itself, updating the stats of equipment, ships, NPCs, "
                    "and everything in between.\n"
                    "Periodically when the game deploys a new patch, it will pause and allow you to read what has changed.");
            }

            ImGui::Checkbox("Count Down On Bases", &config->countDownWhileOnBases);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("If enabled the patch timer will not pause while on a base.");
            }

            ImGui::DragFloat("Time Between Patches (minutes)", &config->timeBetweenPatchesInMinutes, 1.0f, 1.0f, 60.0f, "%.2f");
            ImGui::DragInt("Changes Per Patch (Min)", reinterpret_cast<int*>(&config->changesPerPatchMin), 1.0f, 1, 30);
            ImGui::DragInt("Changes Per Minor (Min)", reinterpret_cast<int*>(&config->changesPerMinorMin), 1.0f, 15, 50);
            ImGui::DragInt("Changes Per Major (Min)", reinterpret_cast<int*>(&config->changesPerMajorMin), 1.0f, 30, 100);

            if (ImGui::Button("View Patch Notes"))
            {
                ImGuiManager::ShowPatchNotes();
            }

            ImGui::SameLine();
            if (ImGui::Button("DEV: Generate New Patch"))
            {
                PatchNotes::GeneratePatch();
            }

            ImGui::SameLine();
            if (ImGui::Button("DEV: Reset Patches"))
            {
                PatchNotes::ResetPatches(false, true);
            }

            ImGui::End();
        }

    public:
        Configurator() = delete;
};
