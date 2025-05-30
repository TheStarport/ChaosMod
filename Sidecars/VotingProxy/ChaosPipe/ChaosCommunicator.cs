using System.Net.NetworkInformation;
using System.Text.Json;
using System.Text.Json.Serialization;
using NetMQ;
using NetMQ.Sockets;
using Serilog;

namespace ChaosMod.VotingProxy.ChaosPipe;

internal class ChaosCommunicator : IChaosCommunicator, IDisposable
{
    public bool HandshakeCompleted { get; private set; }
    public event EventHandler<GetCurrentVotesArgs>? OnGetCurrentVotes;
    public event EventHandler<GetVoteResultArgs>? OnGetVoteResult;
    public event EventHandler<NewVoteArgs>? OnNewVote;


    private readonly ILogger _logger = Log.Logger.ForContext<ChaosCommunicator>();
    private readonly ResponseSocket _socket;

    private readonly JsonSerializerOptions _jsonSerializerOptions = new()
    {
        AllowTrailingCommas = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        Converters =
        {
            new JsonStringEnumConverter()
        }
    };

    public ChaosCommunicator()
    {
        // If the port is already in use 'connect' (>), otherwise 'bind' (@)
        var connectOrBind = PortInUse(5657) ? '>' : '@';
        var uri = $"{connectOrBind}tcp://localhost:5657";
        _socket = new ResponseSocket(uri);
    }

    private void GetCurrentVotes()
    {
        var args = new GetCurrentVotesArgs();

        OnGetCurrentVotes?.Invoke(this, args);

        if (args.CurrentVotes == null)
        {
            _logger.Error("Listeners failed to supply on get current vote args");
        }
        else
        {
            CurrentVotesResult res = new(args.CurrentVotes);
            PushMessage(JsonSerializer.Serialize(res, _jsonSerializerOptions));
        }
    }

    /// <summary>
    ///     Gets called when the chaos mod requests vote results
    /// </summary>
    private void GetVoteResult()
    {
        _logger.Debug("Asking listeners for vote result");
        var args = new GetVoteResultArgs();

        OnGetVoteResult?.Invoke(this, args);

        // Send the chosen option to the pipe
        if (args.ChosenOption == null)
        {
            _logger.Warning("Get vote result did not update chosen option, using 0 (first option)");
            args.ChosenOption = 0;
        }

        VoteResultObject result = new(args.ChosenOption);
        PushMessage(JsonSerializer.Serialize(result, _jsonSerializerOptions));
        _logger.Debug("Vote result sent to pipe: {ArgsChosenOption}", args.ChosenOption);
    }

    /// <summary>
    ///     Reads the contents of the chaos mod pipe and evaluates its message
    /// </summary>
    public async Task PullMessage(CancellationToken token)
    {
        while (!token.IsCancellationRequested)
        {
            // Wait until we have a message
            if (!_socket.HasIn)
            {
                await Task.Delay(100, token);
                continue;
            }

            var msgStr = _socket.ReceiveFrameString();
            _logger.Debug("Received Message {msgStr}", msgStr);

            PipeMessage msg;
            try
            {
                msg = JsonSerializer.Deserialize<PipeMessage>(msgStr, _jsonSerializerOptions)!;
            }
            catch (Exception ex)
            {
                _logger.Error(ex, "Received garbage message");
                continue;
            }

            switch (msg.Identifier)
            {
                case "hello":
                    HandshakeCompleted = true;
                    PushMessage("hello back");
                    break;
                case "ping":
                    PushMessage("pong");
                    break;
                case "vote":
                    StartNewVote(msg.Options);
                    PushMessage("pong");
                    break;
                case "getvoteresult":
                    GetVoteResult();
                    break;
                case "getcurrentvotes":
                    GetCurrentVotes();
                    break;
                default:
                    _logger.Warning("Unknown request: {MsgIdentifier}", msg.Identifier);
                    break;
            }
        }
    }

    /// <summary>
    ///     Sends a message to the chaos mod pipe
    /// </summary>
    /// <param name="message">Message to be sent</param>
    private void PushMessage(string message)
    {
        _socket.SendFrame(message);
    }

    /// <summary>
    ///     Is called when the chaos mod starts a new vote
    /// </summary>
    /// <param name="options">Message from the pipe to be parsed into votes</param>
    private void StartNewVote(List<string>? options)
    {
        if (options is null)
        {
            return;
        }

        // Dispatch information to listeners
        OnNewVote?.Invoke(this, new NewVoteArgs(options.ToArray()));
    }

    /// <summary>
    ///     Sends an error message to the mod and stops voting mode
    /// </summary>
    /// <param name="message">Message to be sent</param>
    public void SendErrorMessage(string message)
    {
        var error = new ErrorObject(message);
        PushMessage(JsonSerializer.Serialize(error));
    }

    public void Dispose()
    {
        _socket.Dispose();
    }

    private static bool PortInUse(int port)
    {
        var ipProperties = IPGlobalProperties.GetIPGlobalProperties();
        var ipEndPoints = ipProperties.GetActiveTcpListeners();
        return ipEndPoints.Any(endPoint => endPoint.Port == port);
    }
}