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
    Unknown,
    Ok,
    BadConfig,
    PipeError
}

internal class VotingProxy
{
    private static void Exit(ExitCode code)
    {
        Environment.Exit((int)code);
    }

    private static readonly ILogger Logger = Log.Logger.ForContext<VotingProxy>();

    private static async Task Main(string[] args)
    {
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
                outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
            .WriteTo.Console(
                outputTemplate: "{Timestamp:yyyy-MM-dd HH:mm:ss.fff zzz} [{Level:u3}] [{SourceContext:l}] {Message:lj}{NewLine}{Exception}")
            .CreateLogger();
        Logger.Information("Starting Freelancer: Chaos Mod - Twitch Proxy");
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
            Logger.Fatal("Unable to deserialize JSON configuration.");
            Exit(ExitCode.BadConfig);
            return;
        }

        await File.WriteAllBytesAsync(jsonPath, JsonSerializer.SerializeToUtf8Bytes(config, jsonSerializerOptions));

        if (args.Length < 1 || args[0] != "--startProxy")
        {
            Logger.Fatal("Please don't start the voting proxy process manually as it's only supposed to be launched by the mod itself."
                         + "\nPass --startProxy as an argument if you want to start the proxy yourself for debugging purposes.");

            Console.ReadKey();
            Exit(ExitCode.BadConfig);
        }

        var mutex = new Mutex(false, "ChaosModVotingMutex");
        mutex.WaitOne();

        // Create components
        Logger.Information("Starting chaos pipe and waiting for connection...");
        var chaosPipe = new ChaosPipeClient();
        if (!chaosPipe.IsConnected())
        {
            Logger.Fatal("Unable to connect to Chaos Pipe. Terminating.");
            Exit(ExitCode.PipeError);
        }

        var votingReceivers = new List<(string Name, IVotingReceiver VotingReceiver)>();

        if (config.ChaosSettings.EnableTwitchVoting)
        {
            var twitch = await TwitchState.Create();
            if (!twitch.Validated && !await twitch.Login())
            {
                Logger.Error("Unable to login to Twitch. Terminating.");
            }
            else
            {
                votingReceivers.Add(("Twitch", new TwitchVotingReceiver(config, chaosPipe, twitch)));
            }
        }

        if (votingReceivers.Count == 0)
        {
            Logger.Fatal("No voting receivers found.");
            Exit(ExitCode.BadConfig);
        }

        foreach (var votingReceiver in votingReceivers)
        {
            Logger.Information($"Initializing {votingReceiver.Name} voting");

            try
            {
                if (await votingReceiver.VotingReceiver.Init())
                {
                    continue;
                }

                Logger.Fatal($"Failed to initialize {votingReceiver.Name} voting");
                return;
            }
            catch (Exception exception)
            {
                Logger.Fatal($"Failed to initialize {votingReceiver.Name} voting\nException occured: ${exception}");
                chaosPipe.SendErrorMessage($"Error occured while initializing {votingReceiver.Name} voting." +
                                           $" Check chaosproxy.log for details.");

                return;
            }
        }

        // Start the chaos mod controller
        Logger.Information("Initializing controller");

        _ = new ChaosModController(chaosPipe, votingReceivers.Select(item => item.VotingReceiver).ToArray(),
            config);

        Logger.Information("Sending hello to mod");

        chaosPipe.SendMessageToPipe("hello");
        while (!chaosPipe.GotHelloBack && chaosPipe.IsConnected())
        {
            await Task.Delay(100);
        }

        if (chaosPipe.GotHelloBack)
        {
            Logger.Information("Received hello_back from mod!");
        }

        while (chaosPipe.IsConnected())
        {
            await Task.Delay(100);
        }

        Logger.Information("Pipe disconnected, ending program.");
        Exit(ExitCode.Ok);
    }

    private static void UnhandledExceptionTrapper(object sender, UnhandledExceptionEventArgs e)
    {
        Logger.Fatal(e.ExceptionObject as Exception, "Fatal exception within the voting proxy.");
        Console.ReadLine();
        Exit(ExitCode.Unknown);
    }
}