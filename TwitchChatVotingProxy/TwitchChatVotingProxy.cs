using System.Text.Json;
using System.Text.Json.Serialization;
using Serilog;
using TwitchChatVotingProxy.ChaosPipe;
using TwitchChatVotingProxy.VotingReceiver;

namespace TwitchChatVotingProxy;

internal enum ExitCode
{
    Unknown,
    Ok, 
    BadConfig,
    PipeError
}

internal class TwitchChatVotingProxy
{
    private static void Exit(ExitCode code)
    {
        Environment.Exit((int)code);
    }
    
    private static readonly ILogger Logger = Log.Logger.ForContext<TwitchChatVotingProxy>();

    private static async Task Main(string[] args)
    {
        System.AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionTrapper;
        
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
        var jsonSerializerOptions = new JsonSerializerOptions()
        {
            WriteIndented = true,
            AllowTrailingCommas = true,
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            Converters =
            {
                new JsonStringEnumConverter()
            }
        };
        
        Configuration? config = null;
        if (File.Exists(jsonPath))
        {
            try
            {
                var jsonRaw = await File.ReadAllTextAsync(jsonPath);
                config = JsonSerializer.Deserialize<Configuration>(jsonRaw, jsonSerializerOptions);
                Logger.Information("Json config serialized");
            }
            catch (Exception ex)
            {
                Logger.Error(ex, "Unable to deserialize JSON configuration.");
                config = null;
            }
        }
        
        config ??= new Configuration();
        await File.WriteAllBytesAsync(jsonPath, JsonSerializer.SerializeToUtf8Bytes(config, jsonSerializerOptions));

        if (args.Length < 1 || args[0] != "--startProxy")
        {
            Logger.Fatal("Please don't start the voting proxy process manually as it's only supposed to be launched by the mod itself."
                              + "\nPass --startProxy as an argument if you want to start the proxy yourself for debugging purposes.");

            Console.ReadKey();
            Exit(ExitCode.BadConfig);
        }   
        
        if (!config.TwitchEnabled)
        {
            Logger.Fatal("Twitch credentials were missing from {jsonPath} " +
                         "(or this was first time running)", jsonPath);
            Console.Read();
            Exit(ExitCode.BadConfig);
        }

        var mutex = new Mutex(false, "ChaosModVotingMutex");
        mutex.WaitOne();

        // Create components
        var chaosPipe = new ChaosPipeClient();
        if (!chaosPipe.IsConnected())
        {
            Logger.Fatal("Unable to connect to Chaos Pipe. Terminating.");
            Exit(ExitCode.PipeError);
        }

        var votingReceivers = new List<(string Name, IVotingReceiver VotingReceiver)>();
        
        if (config.TwitchEnabled)
        {
            votingReceivers.Add(("Twitch", new TwitchVotingReceiver(config, chaosPipe)));
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