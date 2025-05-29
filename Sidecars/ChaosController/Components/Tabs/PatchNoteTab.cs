using ChaosMod.Common;
using ImGuiNET;

namespace ChaosMod.ChaosController.Components.Tabs;

public static partial class Tabs
{
    public static void DisplayPatchNoteTab(ChaosModConfig? config)
    {
        if (config is null)
        {
            return;
        }

        ImGui.NewLine();
        ImGui.TextWrapped(
            "Patch Notes is a feature where the game will frequently write updates to itself, updating the stats of equipment, ships, NPCs, " +
            "and everything in between. Periodically when the game deploys a new patch, it will pause and allow you to read what has changed.");
        ImGui.NewLine();

        ImGui.Checkbox("Enable Patch Notes", ref config.PatchNotes.Enable);
        ImGui.Checkbox("Display Colour Notes", ref config.PatchNotes.DisplayInColor);
        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip("Every change within the patch note system can be positive, negative, or neither.\nIf enabled, they will be colour coded to " +
                             "green, red, and yellow respectively.");
        }

        ImGui.Checkbox("Count Down On Bases", ref config.PatchNotes.CountDownWhileOnBases);
        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip("If enabled the patch timer will not pause while on a base.");
        }

        const float maxTime = 60 * 12;
        ImGui.DragFloat("Time Between Patches (minutes)", ref config.PatchNotes.TimeBetweenPatchesInMinutes, 5.0f, 5.0f, maxTime, "%.2f");
        ImGui.DragInt("Changes Per Patch (Min)", ref config.PatchNotes.ChangesPerPatchMin, 1.0f, 1, 30);
        ImGui.DragInt("Changes Per Minor (Min)", ref config.PatchNotes.ChangesPerMinorMin, 1.0f, 15, 50);
        ImGui.DragInt("Changes Per Major (Min)", ref config.PatchNotes.ChangesPerMajorMin, 1.0f, 30, 100);
    }
}