using ChaosMod.Common;
using ImGuiNET;

namespace ChaosMod.ChaosController.Components.Tabs;

public static partial class Tabs
{
    public static void DisplayDiscordTab(ChaosModConfig? config)
    {
        if (config is null)
        {
            return;
        }

        ImGui.NewLine();

        ImGui.TextWrapped(
            "ChaosMod comes with Discord Rich Presence support. This allows those using Discord to have chaos mod appear on their profile as the active" +
            " activity while playing the game.");
        ImGui.NewLine();

        ImGui.Checkbox("Enable Discord Rich Presence", ref config.DiscordSettings.Enable);

        var options = new[]
        {
            "Time Playing",
            "Time Until Next Chaos Effect",
            "Time Until Next Patch Note"
        };

        var index = (int)config.DiscordSettings.TimerType;
        ImGui.Combo("Discord Timer Type:", ref index, options, options.Length);
        config.DiscordSettings.TimerType = (TimerType)index;

        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip("You can configure what kind of timer is displayed on the rich presence by changing this. " +
                             "By default it will simply say how long you have played.");
        }
    }
}