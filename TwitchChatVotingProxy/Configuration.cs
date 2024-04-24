using System.Text.Json.Serialization;

namespace TwitchChatVotingProxy;

public enum VotingMode
{
    Majority,
    Percentage,
}

[Serializable]
public class Configuration
{
    public VotingMode VotingMode { get; set; } = VotingMode.Majority;
    public bool RetainVotes { get; set; }
    public string? TwitchUsername { get; set; }
    public string? TwitchChannelName { get; set; }
    public string? TwitchToken { get; set; }
    public List<string> PermittedUsernames { get; set; } = new();
    public string? VotingPrefix { get; set; }
    public bool SendMessageUpdates { get; set; }

    [JsonIgnore]
    public bool TwitchEnabled => !(string.IsNullOrEmpty(TwitchUsername) || string.IsNullOrEmpty(TwitchChannelName)
                                                                        || string.IsNullOrEmpty(TwitchToken));
}