#pragma once

class ImGuiManager;
class IntroWindow final
{
        friend ImGuiManager;

        static void Render()
        {
            if (!Get<ConfigManager>()->firstTime)
            {
                return;
            }

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(800, 0.0f), ImGuiCond_Always);
            if (!ImGui::Begin("Welcome to ChaosMod", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
            {
                ImGui::End();
                return;
            }

            ImGui::TextWrapped(
                "Welcome to ChaosMod. Chaos Mod offers a suite of tools to make Freelancer more whacky, amusing, and just overall more chaotic. As "
                "it stands ChaosMod currently offers the following features:\n\n"
                "- Chaos Timer - Periodically the game will trigger a chaotic effect\n"
                "- Patch Notes - Periodically the game will self-patch, permanantely altering weapon and ship stats\n"
                "- Autosaves - A seperate autosave system that will make backups while in space to prevent soft locks\n\n"
                "By default, none of these features are enabled. Starting a new game now will be mostly vanilla, except with with various mission tweaks and "
                "improved AI. All features can be toggled at will, even during a play-through however, unless changed, things like patch notes will "
                "persist.\n\n"
                "The mod can be configured at any time by using 'Mouse 3' to open the wheel menu. The wheel menu provides access to DEV/Debug tools, as well "
                "as the console and configuration menus. The key to open the wheel menu can be "
                "rebound at any time through the CHAOS section of the controls section of the base-game options menu.\n\n"
                "WARNING: This mod is a work in progress, and unstable by its nature. Once any of its features are enabled, stabilty cannot be guaranteed. If "
                "you update the mod at any point, saves are not guranteed to be compatable/stable, and config settings or patch notes might be reset.");

            constexpr std::string_view text = "Understood!";
            const auto windowWidth = ImGui::GetWindowSize().x;
            const auto textWidth = ImGui::CalcTextSize(text.data()).x;

            ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
            if (ImGui::Button(text.data()))
            {
                const auto config = Get<ConfigManager>();
                config->firstTime = false;
                config->Save();
            }

            ImGui::End();
        }
};
