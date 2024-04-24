using System.IO.Pipes;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Timers;
using Serilog;
using Timer = System.Timers.Timer;

namespace TwitchChatVotingProxy.ChaosPipe;

internal class ChaosPipeClient : IChaosPipeClient
{
    /// <summary>
    /// Speed at which the chaos mod pipe gets processed
    /// </summary>
    private const int PipeTickRate = 100;

    public bool GotHelloBack { get; private set; }

    public event EventHandler<GetCurrentVotesArgs>? OnGetCurrentVotes;
    public event EventHandler<GetVoteResultArgs>? OnGetVoteResult;
    public event EventHandler<NewVoteArgs>? OnNewVote;

    private readonly ILogger _logger = Log.Logger.ForContext<ChaosPipeClient>();
    private readonly NamedPipeClientStream _pipe = new(
        ".",
        "ChaosModVotingPipe",
        PipeDirection.InOut,
        PipeOptions.Asynchronous);
    
    private readonly StreamReader? _pipeReader;
    private readonly Timer _pipeTick = new();
    private readonly StreamWriter? _pipeWriter;
    private Task<string?>? _readPipeTask;
    
    private readonly JsonSerializerOptions _jsonSerializerOptions = new()
    {
        AllowTrailingCommas = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        Converters =
        {
            new JsonStringEnumConverter()
        }
    };

    public ChaosPipeClient()
    {
        // Setup pipe tick
        _pipeTick.Interval = PipeTickRate;
        _pipeTick.Elapsed += PipeTick;

        // Connect to the chaos mod pipe
        try
        {
            #if DEBUG
                _pipe.Connect(Timeout.Infinite);
            #else
                _pipe.Connect(5000);
            #endif
            _pipeReader = new StreamReader(_pipe);
            _pipeWriter = new StreamWriter(_pipe)
            {
                AutoFlush = true
            };

            _logger.Information("Successfully connected to chaos mod pipe");

            _pipeTick.Start();
        }
        catch (Exception exception)
        {
            _logger.Fatal(exception, "Failed to connect to chaos mod pipe, aborting");
        }
    }

    /// <summary>
    /// Check if the chaos mod pipe is still connected
    /// </summary>
    /// <returns>If the chaos mod pipe is still connected</returns>
    public bool IsConnected()
    {
        return _pipe.IsConnected;
    }

    /// <summary>
    /// Disconnects the stream reader/writer and the pipe itself
    /// </summary>
    private void DisconnectFromPipe()
    {
        _pipeTick.Stop();
        _pipeTick.Close();

        try
        {
            _pipeReader?.Close();
            _pipeWriter?.Close();
        }
        catch (ObjectDisposedException)
        {

        }
        
        _pipe.Close();
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
            SendMessageToPipe(JsonSerializer.Serialize(res, _jsonSerializerOptions));
        }
    }
    /// <summary>
    /// Gets called when the chaos mod requests vote results
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
        SendMessageToPipe(JsonSerializer.Serialize(result, _jsonSerializerOptions));
        _logger.Debug($"Vote result sent to pipe: {args.ChosenOption}");
    }
    
    /// <summary>
    /// Gets called every pipe tick
    /// </summary>
    private void PipeTick(object? sender, ElapsedEventArgs e)
    {
        try
        {
            SendHeartBeat();
            ReadPipe();
        }
        catch (Exception exception)
        {
            _logger.Information("Pipe disconnected: " + exception.Message);
            DisconnectFromPipe();
        }
    }
    
    /// <summary>
    /// Reads the contents of the chaos mod pipe and evaluates its message
    /// </summary>
    private void ReadPipe()
    {
        // If no reading task is active, create one
        if (_readPipeTask == null)
        {
            _readPipeTask = _pipeReader?.ReadLineAsync();
        }
        // If the reading task is created and complete, get its results
        else if (_readPipeTask.IsCompleted)
        {
            // Get the message from the pipe read
            var message = _readPipeTask.Result;
            if (message is null)
            {
                return;
            }

            // Null the reading task so the next read is dispatched
            _readPipeTask = null;

            // Evaluate message
            var pipe = JsonSerializer.Deserialize<PipeMessage>(message, _jsonSerializerOptions);
            switch (pipe?.Identifier)
            {
                case "hello_back":
                    GotHelloBack = true;
                    break;
                case "vote":
                    StartNewVote(pipe.Options);
                    break;
                case "getvoteresult":
                    GetVoteResult();
                    break;
                case "getcurrentvotes":
                    GetCurrentVotes();
                    break;
                default:
                    _logger.Warning($"Unknown request: {message}");
                    break;
            }
        }
    }
    
    /// <summary>
    /// Sends a message to the chaos mod pipe
    /// </summary>
    /// <param name="message">Message to be sent</param>
    public void SendMessageToPipe(string message)
    {
        _pipeWriter?.Write($"{message}\0");
        _pipe.WaitForPipeDrain();
    }
    
    /// <summary>
    /// Is called when the chaos mod starts a new vote
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
    /// Sends a heartbeat to the chaos mod
    /// </summary>
    private void SendHeartBeat()
    {
        SendMessageToPipe("ping");
    }
    
    /// <summary>
    /// Sends an error message to the mod and stops voting mode
    /// </summary>
    /// <param name="message">Message to be sent</param>
    public void SendErrorMessage(string message)
    {
        var error = new ErrorObject(message);
        SendMessageToPipe(JsonSerializer.Serialize(error));

        DisconnectFromPipe();
    }
}