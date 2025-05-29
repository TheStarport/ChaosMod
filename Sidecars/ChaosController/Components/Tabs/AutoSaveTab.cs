using ChaosMod.Common;
using ImGuiNET;

namespace ChaosMod.ChaosController.Components.Tabs;

public static partial class Tabs
{
    public static void DisplayAutoSaveTab(ChaosModConfig? config)
    {
        if (config is null)
        {
            return;
        }

        ImGui.NewLine();
        ImGui.TextWrapped("Chaos Mod, by its nature, is unstable. The game might crash or soft lock you. It might set you to be hostile to everyone, or " +
                          "remove all your money. For this reason, the game can auto save (even in space) frequently to give you plenty of places to return to during gameplay.");
        ImGui.NewLine();

        ImGui.Checkbox("Enable Custom Autosaves", ref config.AutoSaveSettings.Enable);
        ImGui.SliderInt("Time Between Auto Saves", ref config.AutoSaveSettings.TimeBetweenSavesInSeconds, 60 * 3, 60 * 10);

        ImGui.Checkbox("Allow Saving During Combat", ref config.AutoSaveSettings.AllowAutoSavesDuringCombat);
        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip(
                "By default, the game will not autosave when hostiles are within 2k of the player. If they are it will push the timer back by 30 seconds.\n" +
                "This is done because this can break mission scripts and NPC spawns in some cases. You can enable it if you want, but you've been warned!");
        }
    }
}