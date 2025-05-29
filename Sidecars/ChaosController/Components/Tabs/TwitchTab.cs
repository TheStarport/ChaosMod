using ChaosMod.Common;
using ChaosMod.Common.Twitch;
using ImGuiNET;

namespace ChaosMod.ChaosController.Components.Tabs;

public static partial class Tabs
{
    private static readonly TwitchState twitchState = TwitchState.Create().Result;
    private static Task<bool> twitchLoginTask = Task.FromResult(false);

    public static void DisplayTwitchTab(ChaosModConfig? config)
    {
        if (config is null)
        {
            return;
        }

        ImGui.Checkbox("Enable Twitch Voting", ref config.ChaosSettings.EnableTwitchVoting);
        ImGui.SliderFloat("Twitch Voting Weight", ref config.ChaosSettings.BaseTwitchVoteWeight, 0.1f, 1.0f, "%.2f");
        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip(
                "When using Twitch votes, the item with the most votes will have this percentage chance of being selected.\n This value will determine " +
                "the percentage chance of the Twitch value being selected. If set to 1, Twitch chat's option is guaranteed.\n This option is likely to " +
                "be more preferable if you have fewer engaged viewers as to still allow for a degree of randomness.");
        }

        var options = new[]
        {
            "Do Not Show",
            "Total Only",
            "Vote Per Effect (Total)",
            "Vote Per Effect (Percentage)"
        };

        var index = (int)config.ChaosSettings.TwitchVoteVisibility;
        ImGui.Combo("Twitch Vote Visibility:", ref index, options, options.Length);
        config.ChaosSettings.TwitchVoteVisibility = (TwitchVoteVisibility)index;

        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip(
                "When using Twitch votes, you can decide how visible votes shall be:\n\n" +
                "Do Not Show:                  Do not display how many votes have been cast for the next effect. Total surprise.\n" +
                "Total Only:                   Display only the amount of votes that have been cast, but not what they have been cast for.\n" +
                "Vote Per Effect (Total):      Display how many votes in total have been cast for each potential effect.\n" +
                "Vote Per Effect (Percentage): Display how many votes as a percentage have been cast for each potential effect.");
        }

        var votingModeOptions = new[]
        {
            "Majority",
            "Percentage"
        };

        index = (int)config.ChaosSettings.VotingMode;
        ImGui.Combo("Twitch Vote Type:", ref index, votingModeOptions, votingModeOptions.Length);
        config.ChaosSettings.VotingMode = (VotingMode)index;

        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip(
                "When using Twitch votes, you can decide how their votes influence the result:\n\n" +
                "Majority:    When the vote concludes, the option with the most votes will win (First-past-the-post voting).\n" +
                "Percentage:  Each vote on an option increases the chance that option will be picked (If there are two votes, those two options have a 50% chance of being picked)");
        }

        ImGui.InputTextWithHint("Voting Prefix", "A character that must be put before a number or command, e.g. !", ref config.ChaosSettings.VotingPrefix, 1);
        ImGui.Checkbox("Send Message Updates", ref config.ChaosSettings.SendMessageUpdates);

        ImGui.NewLine();
        var loggedIn = twitchLoginTask is { IsCompleted: true, Result: true };
        ImGui.BeginDisabled(!twitchLoginTask.IsCompleted || twitchLoginTask.Result);
        if (ImGui.Button((loggedIn ? "Logged In " : "Login to Twitch ") + BrandIcon.Twitch))
        {
            twitchLoginTask = twitchState.Login();
        }

        if (!twitchLoginTask.IsCompleted)
        {
            ImGui.SameLine();
            ImGui.Text("Opening in browser... complete login there");
        }

        ImGui.EndDisabled();
        if (loggedIn)
        {
            ImGui.Text("Successfully logged in to Twitch. Token has been saved and can now be used in ChaosMod.");
        }

        if (ImGui.IsItemHovered())
        {
            ImGui.SetTooltip("Once logged in to Twitch, a login token will be securely stored. Once this is done, you can start the voting proxy within " +
                             "ChaosMod.");
        }
    }
}