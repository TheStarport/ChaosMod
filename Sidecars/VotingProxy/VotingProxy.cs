using System.Text.Json;
using System.Text.Json.Serialization;
using ChaosMod.Common;
using ChaosMod.Common.Twitch;
using ChaosMod.VotingProxy.ChaosPipe;
using ChaosMod.VotingProxy.VotingReceiver;
using Serilog;

namespace ChaosMod.VotingProxy;

internal enum ExitCode
{
    Ok,
    Unknown,
    BadConfig,
    SocketError,
    AlreadyRunning
}

internal class VotingProxy
{
    private static ChaosCommunicator? _chaosCommunicator;
    private static void Exit(ExitCode code)
    {
        _chaosCommunicator?.Dispose();
        Environment.Exit((int)code);
    }

    private static ILogger _logger = null!;

    private static async Task Main(string[] args)
    {
        // ReSharper disable once NotAccessedVariable
        var mutex = new Mutex(true, "ChaosModVotingMutex", out var @new);
        if (@new)
        {
            Exit(ExitCode.AlreadyRunning);
        }

        AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionTrapper;

        var logPath = Path.Join(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
            "My Games", "FLChaosMod", "VotingProxy.log");

        Log.Logger = new LoggerConfiguration()
            #if DEBUG
            .MinimumLevel.Debug()
            #else
            .MinimumLevel.Information()
            #endif
            .WriteTo.File(logPath,
                outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}",
                flushToDiskInterval: TimeSpan.FromSeconds(1))
            .WriteTo.Console(
                outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
            .CreateLogger();

        _logger = Log.Logger.ForContext<VotingProxy>();
        _logger.Information("Starting Freelancer: Chaos Mod - Twitch Proxy");
        var jsonPath = Path.Join(Directory.GetCurrentDirectory(), "ChaosModVoting.json");
        var jsonSerializerOptions = new JsonSerializerOptions
        {
            WriteIndented = true,
            AllowTrailingCommas = true,
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            Converters =
            {
                new JsonStringEnumConverter()
            }
        };
        ;

        var config = ChaosModConfig.Load(false);
        if (config is null)
        {
            _logger.Fatal("Unable to deserialize JSON configuration.");
            Exit(ExitCode.BadConfig);
            return;
        }

        await File.WriteAllBytesAsync(jsonPath, JsonSerializer.SerializeToUtf8Bytes(config, jsonSerializerOptions));

        if (args.Length < 1 || args[0] != "--startProxy")
        {
            _logger.Fatal("Please don't start the voting proxy process manually as it's only supposed to be launched by the mod itself."
                          + "\nPass --startProxy as an argument if you want to start the proxy yourself for debugging purposes.");

            Console.ReadKey();
            Exit(ExitCode.BadConfig);
        }

        // Setup socket connections
        _logger.Information("Starting chaos sockets and waiting for connection...");

        try
        {
            _chaosCommunicator = new ChaosCommunicator();
        }
        catch (Exception ex)
        {
            _logger.Fatal(ex, "Unable to connect/bind to Chaos ports");
            Exit(ExitCode.SocketError);
            return;
        }

        var pullMessageCts = new CancellationTokenSource();
        var pullMessageTask = Task.Run(() => _chaosCommunicator.PullMessage(pullMessageCts.Token), pullMessageCts.Token);

        var votingReceivers = new List<(string Name, IVotingReceiver VotingReceiver)>();

        if (config.ChaosSettings.EnableTwitchVoting)
        {
            var twitch = await TwitchState.Create();
            votingReceivers.Add(("Twitch", new TwitchVotingReceiver(config, _chaosCommunicator, twitch)));
        }

        if (votingReceivers.Count == 0)
        {
            _logger.Fatal("No voting receivers found.");
            Exit(ExitCode.BadConfig);
        }

        foreach (var votingReceiver in votingReceivers)
        {
            _logger.Information("Initializing {VotingReceiverName} voting", votingReceiver.Name);

            try
            {
                if (await votingReceiver.VotingReceiver.Init())
                {
                    continue;
                }

                _logger.Fatal("Failed to initialize {VotingReceiverName} voting", votingReceiver.Name);
                return;
            }
            catch (Exception exception)
            {
                _logger.Fatal("Failed to initialize {VotingReceiverName} voting\nException occured: ${Exception}", votingReceiver.Name, exception);
                _chaosCommunicator.SendErrorMessage($"Error occured while initializing {votingReceiver.Name} voting." +
                                                    $" Check VotingProxy.log for details.");

                return;
            }
        }

        // Start the chaos mod controller
        _logger.Information("Initializing controller");

        _ = new ChaosModController(_chaosCommunicator, votingReceivers.Select(item => item.VotingReceiver).ToArray(),
            config);

        if (_chaosCommunicator.HandshakeCompleted)
        {
            _logger.Information("Received 'hello back' from mod!");
        }

        while (!pullMessageTask.IsCompleted)
        {
            await Task.Delay(100, pullMessageCts.Token);
        }

        _logger.Information("Chaos sockets disconnected, ending program.");
        mutex.ReleaseMutex();
        Exit(ExitCode.Ok);
    }

    private static void UnhandledExceptionTrapper(object sender, UnhandledExceptionEventArgs e)
    {
        _logger.Fatal(e.ExceptionObject as Exception, "Fatal exception within the voting proxy.");
        Console.ReadLine();
        Exit(ExitCode.Unknown);
    }
}