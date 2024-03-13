#pragma once

#include "../Components/PiMenu.hpp"
#include "Systems/SystemComponents/GlobalTimers.hpp"
#include "imgui/imgui.h"

class ImGuiManager;
class SelectionWheel final
{
        friend ImGuiManager;

        inline static bool shouldShow = false;
        inline static bool currentlyShowing = false;

        static void RenderWheel()
        {
            using namespace ImGuiExt;

            if (BeginPiePopup("SelectionMenu"))
            {
                if (PieMenuItem("Configurator"))
                {
                    ImGuiManager::ShowConfigurator();
                }

                if (PieMenuItem("Console"))
                {
                    ImGuiManager::ShowDebugConsole();
                }

                if (PieMenuItem("Effect History"))
                {
                    ImGuiManager::ShowEffectHistory();
                }

                if (PieMenuItem("Show Patch Notes"))
                {
                    ImGuiManager::ShowPatchNotes();
                }

                if (BeginPieMenu("Debug"))
                {
                    if (PieMenuItem("DEV: New Patch"))
                    {
                        PatchNotes::GeneratePatch();
                    }

                    if (PieMenuItem("DEV: Clear Patches"))
                    {
                        PatchNotes::ResetPatches(false, true);
                    }

                    if (PieMenuItem("DEV: Effect Selector"))
                    {
                        ImGuiManager::ShowEffectSelector();
                    }

                    if (PieMenuItem("DEV: Advance Level"))
                    {
                        int money = 0;
                        pub::Player::GetMoneyNeededToNextRank(1, money);
                        if (money > 0)
                        {
                            pub::Player::AdjustCash(1, money);
                            GlobalTimers::i()->AddTimer(
                                [money](const float _)
                                {
                                    pub::Player::AdjustCash(1, -money);
                                    return true;
                                },
                                1.f);
                        }
                    }

                    if (PieMenuItem("DEV: Add Cargo"))
                    {
                        ImGuiManager::ShowCargoSpawner();
                    }

                    EndPieMenu();
                }

                EndPiePopup();
            }
        }

        static void Render()
        {
            if (shouldShow)
            {
                currentlyShowing = true;
            }

            if (!currentlyShowing)
            {
                return;
            }

            if (ImGui::Begin("Selection Wheel Window",
                             &currentlyShowing,
                             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoScrollWithMouse))
            {
                if (shouldShow)
                {
                    shouldShow = false;
                    ImGui::OpenPopup("SelectionMenu");
                }

                RenderWheel();
            }

            ImGui::End();
        }
};
