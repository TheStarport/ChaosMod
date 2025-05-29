using ChaosMod.Common;
using ImGuiNET;

namespace ChaosMod.ChaosController.Components.Tabs;

public static partial class Tabs
{
    public static void DisplayChaosTab(ChaosModConfig? config)
    {
        if (config is null)
        {
            return;
        }

        ImGui.TextWrapped("The 'Chaos' feature, for which the entire mod is named, is a periodic timer that will trigger random effects while in space. " +
                          "This could be beneficial like giving you OP weapons, or it could be detrimental where we spawn hostile NPCs. " +
                          "It could be neither where the effect just makes the universe different or annoying.");

        ImGui.Checkbox("Enable Chaos Timer", ref config.ChaosSettings.Enable);
        ImGui.SliderFloat("Time Between Chaos", ref config.ChaosSettings.TimeBetweenChaos, 5.0f, 120.0f, "%.0f");
        ImGui.SliderFloat("Default Effect Duration", ref config.ChaosSettings.DefaultEffectDuration, 20.0f, 300.0f, "%.0f");
        ImGui.SliderInt("Total Allowed Concurrent Effects", ref config.ChaosSettings.TotalAllowedConcurrentEffects, 1, 12);

        ImGui.Checkbox("Block Teleports During Missions", ref config.ChaosSettings.BlockTeleportsDuringMissions);

        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip("Many missions can softlock if the player is teleports away, into a different system, or to a base. If true, this will " +
                             "prevent teleport effects from being selected while on a mission.");
        }
    }
}