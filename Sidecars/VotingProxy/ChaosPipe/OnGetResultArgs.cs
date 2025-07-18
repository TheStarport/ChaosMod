﻿namespace ChaosMod.VotingProxy.ChaosPipe;

internal class GetVoteResultArgs
{
    /// <summary>
    ///     The chosen option that should be sent to the chaos mod pipe
    /// </summary>
    public int? ChosenOption { get; set; } = null;
}