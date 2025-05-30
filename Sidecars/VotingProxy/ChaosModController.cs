using ChaosMod.Common;
using ChaosMod.VotingProxy.ChaosPipe;
using ChaosMod.VotingProxy.Contracts;
using ChaosMod.VotingProxy.VotingReceiver;

namespace ChaosMod.VotingProxy;

internal class ChaosModController
{
    private readonly IChaosCommunicator _chaosPipe;
    private readonly IVotingReceiver[] _votingReceivers;
    private readonly ChaosModConfig _config;

    private List<IVoteOption> _activeVoteOptions = [];
    private readonly Dictionary<string, int> _userVotedFor = new();
    private readonly Random _random = new();
    private int _voteCounter;
    private bool _voteRunning;

    public ChaosModController(IChaosCommunicator chaosPipe, IVotingReceiver[] votingReceivers,
        ChaosModConfig config)
    {
        _chaosPipe = chaosPipe;
        _votingReceivers = votingReceivers;
        _config = config;

        // Setup pipe listeners
        _chaosPipe.OnGetCurrentVotes += OnGetCurrentVotes;
        _chaosPipe.OnGetVoteResult += OnGetVoteResult;
        _chaosPipe.OnNewVote += OnNewVote;

        // Setup receiver listeners
        foreach (var votingReceiver in _votingReceivers)
        {
            votingReceiver.Message += OnVoteReceiverMessage;
        }
    }

    /// <summary>
    ///     Calculate the voting result by counting them, and returning the one
    ///     with the most votes.
    /// </summary>
    private int GetVoteResultByMajority()
    {
        // Find the highest vote count
        var highestVoteCount = _activeVoteOptions.Max(_ => _.Votes);
        // Get all options that have the highest vote count
        var chosenOptions = _activeVoteOptions.FindAll(_ => _.Votes == highestVoteCount);
        var chosenOption =
            // If we only have one choosen option, use that
            chosenOptions.Count == 1 ? chosenOptions[0] :
                // Otherwise we have more than one option with the same vote count and choose one at random
                chosenOptions[_random.Next(0, chosenOptions.Count)];

        return _activeVoteOptions.IndexOf(chosenOption);
    }

    /// Calculate the voting result by assigning them a percentage based on votes,
    /// and choosing a random option based on that percentage.
    private int GetVoteResultByPercentage()
    {
        // Get total votes
        var votes = _activeVoteOptions.Select(option => option.Votes).ToList();
        var totalVotes = votes.Sum();

        // If we have no votes, choose one at random
        if (totalVotes == 0)
        {
            return _random.Next(0, votes.Count);
        }

        // Select a random vote from all votes
        var selectedVote = _random.Next(1, totalVotes + 1);

        // Now find out in what vote range/option that vote is
        var voteRange = 0;
        var selectedOption = 0;

        for (var i = 0; i < votes.Count; i++)
        {
            voteRange += votes[i];
            if (selectedVote > voteRange)
            {
                continue;
            }

            selectedOption = i;
            break;
        }

        // Return the selected vote range/option
        return selectedOption;
    }

    /// <summary>
    ///     Is called when the chaos mod pipe requests the current votes (callback)
    /// </summary>
    private void OnGetCurrentVotes(object? sender, GetCurrentVotesArgs args)
    {
        args.CurrentVotes = _activeVoteOptions.Select(x => x.Votes).ToList();
    }

    /// <summary>
    ///     Is called when the chaos mod wants to know the voting result (callback)
    /// </summary>
    private void OnGetVoteResult(object? sender, GetVoteResultArgs e)
    {
        // Evaluate what result calculation to use
        e.ChosenOption = _config.ChaosSettings.VotingMode switch
        {
            VotingMode.Majority => GetVoteResultByMajority(),
            VotingMode.Percentage => GetVoteResultByPercentage(),
            _ => e.ChosenOption
        };

        // Vote round ended
        _voteRunning = false;
    }

    /// <summary>
    ///     Is called when the chaos mod start a new vote (callback)
    /// </summary>
    private async void OnNewVote(object? sender, NewVoteArgs e)
    {
        _activeVoteOptions = e.VoteOptionNames
            .ToList()
            .Select(IVoteOption (voteOptionName, index) =>
            {
                // We want the options to alternate between matches.
                // If we are on an even round we basically select the index (+1 for non programmers).
                // If we are on an odd round, we add to the index the option count.
                // This gives us a pattern like following:
                // Round 0: [O1, O2, O3, ...]
                // Round 1: [O4, O5, O6, ...]
                var match = _voteCounter % 2 == 0
                    ? _config.ChaosSettings.VotingPrefix + (index + 1)
                    : _config.ChaosSettings.VotingPrefix + (index + 1 + _activeVoteOptions.Count);

                return new VoteOption(voteOptionName, [match]);
            }).ToList();

        var msg = "Time for a new effect! Vote between:";
        foreach (var voteOption in _activeVoteOptions)
        {
            msg += "\n";

            var firstIndex = true;
            foreach (var match in voteOption.Matches)
            {
                msg += firstIndex ? $"{match} " : $" / {match}";

                firstIndex = true;
            }

            msg += $": {voteOption.Label}";
        }

        if (_config.ChaosSettings.VotingMode == VotingMode.Percentage)
        {
            msg += "\nVotes will affect the chance for one of the effects to occur.";
        }

        foreach (var votingReceiver in _votingReceivers)
        {
            await votingReceiver.SendMessage(msg);
        }


        // Clear the old voted for information
        _userVotedFor.Clear();
        // Increase the vote counter
        _voteCounter++;

        // Vote round started now
        _voteRunning = true;
    }

    /// <summary>
    ///     Is called when the voting receiver receives a message
    /// </summary>
    private void OnVoteReceiverMessage(object? sender, MessageArgs e)
    {
        if (!_voteRunning || e.ClientId is null || e.Message is null)
        {
            return;
        }

        // TODO: Make blacklist!

        for (var i = 0; i < _activeVoteOptions.Count; i++)
        {
            var voteOption = _activeVoteOptions[i];

            if (!voteOption.Matches.Contains(e.Message))
            {
                continue;
            }

            // Check if the player has already voted
            if (!_userVotedFor.TryGetValue(e.ClientId, out var previousVote))
            {
                // If they haven't voted, count his vote
                _userVotedFor.Add(e.ClientId, i);
                voteOption.Votes++;
            }
            else if (previousVote != i)
            {
                // If the player has already voted, and it's not the same as before,
                // remove the old vote, and add the new one.
                _userVotedFor.Remove(e.ClientId);
                _activeVoteOptions[previousVote].Votes--;

                _userVotedFor.Add(e.ClientId, i);
                voteOption.Votes++;
            }

            break;
        }
    }
}