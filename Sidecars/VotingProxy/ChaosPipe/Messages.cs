namespace ChaosMod.VotingProxy.ChaosPipe;

[Serializable]
public class PipeMessage
{
    public string? Identifier { get; set; }
    public List<string>? Options { get; set; }
}

[Serializable]
public class CurrentVotesResult
{
    public string Identifier { get; } = "currentvotes";
    public List<int> Votes { get; }
    public int TotalVotes { get; }
    public List<float> VotePercentages { get; }

    public CurrentVotesResult(List<int> votes)
    {
        Votes = votes;
        TotalVotes = Votes.Aggregate(0, (acc, next) => acc + next);
        VotePercentages = TotalVotes is not 0
            ? Votes.Select(x => (float)x / TotalVotes).ToList()
            : new List<float>
            {
                .25f,
                .25f,
                .25f,
                .25f
            };
    }
}

[Serializable]
public class VoteResultObject
{
    public string Identifier { get; } = "voteresult";
    public int? SelectedOption { get; }

    public VoteResultObject(int? selectedOption)
    {
        SelectedOption = selectedOption;
    }
}

[Serializable]
public class ErrorObject
{
    public string Identifier { get; } = "error";
    public string Message { get; }

    public ErrorObject(string message)
    {
        Message = $"{message} Reverting to normal mode.";
    }
}