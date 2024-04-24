namespace TwitchChatVotingProxy.ChaosPipe;

internal class NewVoteArgs
{
    public string[] VoteOptionNames { get; }

    public NewVoteArgs(string[] voteOptionNames)
    {
        VoteOptionNames = voteOptionNames;
    }
}