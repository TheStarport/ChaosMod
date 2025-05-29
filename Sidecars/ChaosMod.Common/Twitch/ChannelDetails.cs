using TwitchLib.Api.Helix.Models.Channels.GetChannelInformation;
using TwitchLib.Api.Helix.Models.Chat.GetChatters;

namespace ChaosMod.Common.Twitch;

public class ChannelDetails
{
    public required ChannelInformation Information { get; init; }
    public HashSet<Chatter> Chatters { get; init; } = [];
}