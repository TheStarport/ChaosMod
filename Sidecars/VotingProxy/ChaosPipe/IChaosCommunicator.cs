namespace ChaosMod.VotingProxy.ChaosPipe;

internal interface IChaosCommunicator
{
    event EventHandler<GetCurrentVotesArgs> OnGetCurrentVotes;
    /// <summary>
    ///     Event that gets invoked when the chaos mod requests voting results
    /// </summary>
    event EventHandler<GetVoteResultArgs> OnGetVoteResult;
    /// <summary>
    ///     Event that gets invoked when the chaos mod creates a new vote
    /// </summary>
    event EventHandler<NewVoteArgs> OnNewVote;
}