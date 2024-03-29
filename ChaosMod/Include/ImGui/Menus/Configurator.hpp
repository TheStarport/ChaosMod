#pragma once

#include "../ImGuiManager.hpp"

#include "Components/ConfigManager.hpp"
#include "Components/TwitchVoting.hpp"

#include "CoreComponents/PatchNotes.hpp"

#include "imfilebrowser.h"
#include "imgui.h"

#include <magic_enum_utility.hpp>

class ImGuiManager;
class Configurator final
{
        friend ImGuiManager;

        bool show = false;
        bool importer;
        inline static std::map<EffectType, std::vector<ActiveEffect*>>* allEffects;

        inline static DWORD* gamePauseIncrementor = PDWORD(0x667D54);
        inline static bool doDecrement = false;

        std::shared_ptr<ConfigManager> config = nullptr;

        ImGui::FileBrowser fileBrowser{ ImGuiFileBrowserFlags_ConfirmOnEnter };

        bool confirmImport = false;
        std::string exportPath;

        void RenderChaosTab() const
        {
            ImGui::TextWrapped("PLACEHOLDER DESCRIPTION THAT IS SUPER DESCRIPTIVE ABOUT THINGS THAT NEED DESCRIBING FOR THIS DESCRIPTIVE TAB OF DESCRIPTIONS.");

            ImGui::Checkbox("Enable Chaos Timer", &config->chaosSettings.enable);
            ImGui::SliderFloat("Time Between Chaos", &config->chaosSettings.timeBetweenChaos, 5.0f, 120.0f);
            ImGui::SliderFloat("Default Effect Duration", &config->chaosSettings.defaultEffectDuration, 20.0f, 300.0f);
            ImGui::SliderInt("Total Allowed Concurrent Effects", reinterpret_cast<PINT>(&config->chaosSettings.totalAllowedConcurrentEffects), 1, 12);

            ImGui::Checkbox("Block Teleports During Missions", &config->chaosSettings.blockTeleportsDuringMissions);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Many missions can softlock if the player is teleports away, into a different system, or to a base. If true, this will "
                                  "prevent teleport effects from being selected while on a mission.");
            }
        }

        void RenderAutoSaveTab() const
        {
            ImGui::NewLine();
            ImGui::TextWrapped("Chaos Mod, by its nature, is unstable. The game might crash or soft lock you. It might set you to be hostile to everyone, or "
                               "remove all your money."
                               "For this reason, the game can auto save (even in space) frequently to give you plenty of places to return to during gameplay.");
            ImGui::NewLine();

            ImGui::Checkbox("Enable Custom Autosaves", &config->autoSaveSettings.enable);
            ImGui::SliderInt("Time Between Auto Saves", reinterpret_cast<PINT>(&config->autoSaveSettings.timeBetweenSavesInSeconds), 60 * 3, 60 * 10);

            ImGui::Checkbox("Allow Saving During Combat", &config->autoSaveSettings.allowAutoSavesDuringCombat);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "By default, the game will not autosave when hostiles are within 2k of the player. If they are it will push the timer back by 30 seconds.\n"
                    "This is done because this can break mission scripts and NPC spawns in some cases. You can enable it if you want, but you've been warned!");
            }
        }

        void RenderTwitchTab() const
        {
            ImGui::Checkbox("Enable Twitch Voting", &config->chaosSettings.enableTwitchVoting);
            ImGui::SliderFloat("Twitch Voting Weight", &config->chaosSettings.baseTwitchVoteWeight, 0.1f, 1.0f);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "When using Twitch votes, the item with the most votes will have this percentage chance of being selected.\n This value will determine "
                    "the percentage chance of the Twitch value being selected. If set to 1, Twitch chat's option is guaranteed.\n This option is likely to "
                    "be more preferable if you have fewer engaged viewers as to still allow for a degree of randomness.");
            }

            static bool initialized = false;
            ImGui::BeginDisabled(!config->chaosSettings.enableTwitchVoting || initialized);
            if (ImGui::Button("Initialize Voting Proxy") && Get<TwitchVoting>()->Initialize())
            {
                initialized = true;
            }
            ImGui::EndDisabled();
        }

        void RenderStyleTab() const
        {
            static DWORD startingProgressBarColor = config->chaosSettings.progressBarColor;
            static auto startingProgressBarColorVec = ImGui::ColorConvertU32ToFloat4(startingProgressBarColor);
            static float progressColor[3] = { startingProgressBarColorVec.x, startingProgressBarColorVec.y, startingProgressBarColorVec.z };
            ImGui::ColorEdit3("Chaos Progress Color", progressColor, ImGuiColorEditFlags_PickerHueBar);
            config->chaosSettings.progressBarColor = ImGui::ColorConvertFloat4ToU32(ImVec4(progressColor[0], progressColor[1], progressColor[2], 0xFF));

            static DWORD startingProgressBarTextColor = config->chaosSettings.progressBarTextColor;
            static auto startingProgressBarTextColorVec = ImGui::ColorConvertU32ToFloat4(startingProgressBarTextColor);
            static float progressTextColor[3] = { startingProgressBarTextColorVec.x, startingProgressBarTextColorVec.y, startingProgressBarTextColorVec.z };
            ImGui::ColorEdit3("Chaos Progress Text Color", progressTextColor, ImGuiColorEditFlags_PickerHueBar);
            config->chaosSettings.progressBarTextColor =
                ImGui::ColorConvertFloat4ToU32(ImVec4(progressTextColor[0], progressTextColor[1], progressTextColor[2], 0xFF));

            const char* timerOptions[] = { "Progress Bar: Top", "Progress Bar: Sides", "Clock", "Countdown" };

            ImGui::Combo("Chaos Progress Bar Type:", reinterpret_cast<int*>(&config->chaosSettings.progressBarType), timerOptions, IM_ARRAYSIZE(timerOptions));

            ImGui::Checkbox("Show Time Remaining On Effects", &config->chaosSettings.showTimeRemainingOnEffects);
        }

        void RenderRandomTab() const
        {
            ImGui::NewLine();
            ImGui::TextWrapped(
                "Patch Notes is a feature where the game will frequently write updates to itself, updating the stats of equipment, ships, NPCs, "
                "and everything in between. Periodically when the game deploys a new patch, it will pause and allow you to read what has changed.");
            ImGui::NewLine();

            ImGui::Checkbox("Enable Patch Notes", &config->patchNotes.enable);
            ImGui::Checkbox("Display Colour Notes", &config->patchNotes.displayInColor);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Every change within the patch note system can be positive, negative, or neither.\nIf enabled, they will be colour coded to "
                                  "green, red, and yellow respectively.");
            }

            ImGui::Checkbox("Count Down On Bases", &config->patchNotes.countDownWhileOnBases);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("If enabled the patch timer will not pause while on a base.");
            }

            constexpr float maxTime = 60 * 12;
            ImGui::DragFloat("Time Between Patches (minutes)", &config->patchNotes.timeBetweenPatchesInMinutes, 5.0f, 5.0f, maxTime, "%.2f");
            ImGui::DragInt("Changes Per Patch (Min)", reinterpret_cast<int*>(&config->patchNotes.changesPerPatchMin), 1.0f, 1, 30);
            ImGui::DragInt("Changes Per Minor (Min)", reinterpret_cast<int*>(&config->patchNotes.changesPerMinorMin), 1.0f, 15, 50);
            ImGui::DragInt("Changes Per Major (Min)", reinterpret_cast<int*>(&config->patchNotes.changesPerMajorMin), 1.0f, 30, 100);

            static char randomSeed[255];
            ImGui::InputText("Seed", randomSeed, sizeof(randomSeed));

            if (ImGui::Button("Generate Random Seed"))
            {
                const auto rand = Get<Random>();
                const std::string str = std::format("{} {}", rand->RandomAdjective(), rand->RandomNoun());
                std::fill_n(randomSeed, sizeof(randomSeed), '\0');
                strcpy_s(randomSeed, str.c_str());
            }

            ImGui::BeginDisabled(randomSeed[0] == '\0');
            ImGui::SameLine();
            if (ImGui::Button("Use Seed?"))
            {
                PatchNotes::Reseed(std::string_view(randomSeed));
                std::fill_n(randomSeed, sizeof(randomSeed), '\0');
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("WARNING: Changing the seed will reset all patch notes, "
                                  "erasing any that have already been applied and returning the game to a vanilla state!");
            }
            
            ImGui::EndDisabled();
        }

        void RenderEffectToggleTab() const
        {
            auto& configEffects = Get<ConfigManager>()->chaosSettings.toggledEffects;

            magic_enum::enum_for_each<EffectType>(
                [&configEffects](auto val)
                {
                    constexpr EffectType category = val;
                    const auto categoryName = std::string(magic_enum::enum_name(category));

                    const auto categoryEffects = allEffects->find(category);
                    if (categoryEffects == allEffects->end())
                    {
                        return;
                    }

                    bool configEffectsNeedPopulating = false;
                    if (!configEffects.contains(categoryName))
                    {
                        configEffects[categoryName] = {};
                        configEffectsNeedPopulating = true;
                    }

                    auto& configCategoryEffects = configEffects[categoryName];

                    ImGui::PushID(categoryName.c_str());
                    bool all = std::all_of(configCategoryEffects.begin(),
                                           configCategoryEffects.end(),
                                           [](std::pair<const std::string, bool>& individualEffect) { return individualEffect.second; });

                    if (ImGui::Checkbox("All?##ID", &all))
                    {
                        for (auto& i : configCategoryEffects)
                        {
                            i.second = all;
                        }

                        Get<ConfigManager>()->Save();
                    }

                    ImGui::SameLine();
                    ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, ImVec2(0.45f, 0.5f));
                    ImGui::SeparatorText(categoryName.c_str());
                    ImGui::PopStyleVar();

                    ImGui::NewLine();

                    if (ImGui::BeginTable(categoryName.c_str(), 2))
                    {
                        for (ActiveEffect* effect : categoryEffects->second)
                        {
                            auto& info = effect->GetEffectInfo();

                            if (configEffectsNeedPopulating)
                            {
                                configEffects[categoryName][info.effectName] = true;
                            }

                            auto& isEnabled = configEffects[categoryName][info.effectName];

                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(info.effectName.c_str(), &isEnabled))
                            {
                                Get<ConfigManager>()->Save();
                            }

                            if (ImGui::IsItemHovered())
                            {
                                ImGui::SetTooltip(info.description.c_str()); // NOLINT(clang-diagnostic-format-security)
                            }
                        }
                        ImGui::EndTable();
                    }

                    ImGui::NewLine();

                    if (configEffectsNeedPopulating)
                    {
                        Get<ConfigManager>()->Save();
                    }

                    ImGui::PopID();
                });
        }

        void HandleFileBrowser()
        {
            fileBrowser.Display();

            if (!fileBrowser.HasSelected())
            {
                return;
            }

            auto path = fileBrowser.GetSelected();
            fileBrowser.ClearSelected();

            ImGuiManager::ImportConfig(path.generic_string());
            ImGui::OpenPopup("Imported Config");
        }

        void Render()
        {
            HandleFileBrowser();

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

            ImGui::SetNextWindowSize({ 900.f, 800.f }, ImGuiCond_Always);
            if (!ImGui::Begin(importer ? "Config Importer" : "Configurator", &show, ImGuiWindowFlags_NoResize))
            {
                ImGui::End();
                return;
            }

            if (importer)
            {
                if (ImGui::Button("Import"))
                {
                    confirmImport = true;
                }
            }
            else
            {
                ImGui::BeginDisabled(fileBrowser.IsOpened());
                if (ImGui::Button("Save Changes"))
                {
                    config->Save();
                }

                ImGui::SameLine();
                if (ImGui::Button("Export Preset"))
                {
                    exportPath = std::filesystem::current_path().append("presets\\exported.json").string();
                    config->Save(exportPath);
                    ImGui::OpenPopup("Exported Config");
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("TODO: Tooltip explaining how exporting presets works");
                }

                ImGui::SameLine();
                if (ImGui::Button("Import Preset"))
                {
                    static const std::vector<std::string> filters = { ".json", ".txt" };
                    fileBrowser.SetTypeFilters(filters);

                    const auto path = std::filesystem::current_path().append("presets");
                    fileBrowser.SetPwd(exists(path) ? path : std::filesystem::current_path());
                    fileBrowser.Open();
                }

                ImGui::EndDisabled();

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("You can load preset configs. TODO Better explain this later");
                }
            }

            ImGui::Separator();
            ImGui::NewLine();

            if (ImGui::BeginTabBar("Configurator Tabs"))
            {
                if (ImGui::BeginTabItem("Chaos Settings"))
                {
                    ImGui::BeginDisabled(importer);
                    RenderChaosTab();
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Autosave Settings"))
                {
                    ImGui::BeginDisabled(importer);
                    RenderAutoSaveTab();
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Style Settings"))
                {
                    ImGui::BeginDisabled(importer);
                    RenderStyleTab();
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Randomizer Settings"))
                {
                    ImGui::BeginDisabled(importer);
                    RenderRandomTab();
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Twitch Settings"))
                {
                    ImGui::BeginDisabled(importer);
                    RenderTwitchTab();
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Effect Toggles"))
                {
                    ImGui::BeginDisabled(importer);
                    RenderEffectToggleTab();
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Exported Config", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Exported Config To:");
                ImGui::Text(exportPath.c_str());

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
            else if (ImGui::BeginPopupModal("Imported Config", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Config successfully imported");

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::End();
        }

        explicit Configurator(std::shared_ptr<ConfigManager> existingConfig) : importer(existingConfig != nullptr)
        {
            if (!existingConfig)
            {
                config = std::shared_ptr<ConfigManager>(Get<ConfigManager>());
            }
            else
            {
                show = true;
                config = existingConfig;
            }
        }

    public:
        Configurator() = delete;
};
