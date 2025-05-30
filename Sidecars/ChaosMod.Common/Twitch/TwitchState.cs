using System.Diagnostics;
using System.Net;
using ChaosMod.Common.Models;
using ChaosMod.Common.Utils;
using Flurl.Http;
using libc.hwid;
using Serilog;
using TwitchLib.Api;
using TwitchLib.Api.Helix.Models.Users.GetUsers;
using TwitchLib.Client;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Models;
using ILogger = Serilog.ILogger;

namespace ChaosMod.Common.Twitch;

public class TwitchState
{
    private const string ClientId = "m37g5it27hdl2bi3tdece5z9hmmylj";
    private const string AuthUrl = "https://id.twitch.tv/oauth2/device";
    private const string ValidationUrl = "https://id.twitch.tv/oauth2/token";
    private const string TwitchFile = "twitch.key";
    private const string KeySplitter = "FLCHS";

    private static readonly string[] Scopes =
    [
        "chat:read",
        "chat:edit",
        "user:read:chat",
        "channel:manage:polls", // Read/create polls (mirror game polls to channel, sync channel poll to game state, check if a poll is running before making one)
        "channel:moderate", // Needed to get PubSub events when a chat message is deleted by a mod
        "user:bot",
        "channel:bot",
        "user:write:chat",
        "moderator:manage:announcements"
    ];

    private static readonly ILogger Logger = Log.Logger.ForContext<TwitchState>();
    private TwitchState()
    {
    }

    public string AccessToken { get; private set; } = "";
    public string RefreshToken { get; private set; } = "";
    public TwitchAPI? TwitchApi { get; private set; }
    public ChannelDetails? Channel { get; private set; }
    public TwitchClient? Client { get; private set; }
    public User? User { get; private set; }

    public bool Validated => AccessToken != "" && RefreshToken != "";

    public static async Task<TwitchState> Create()
    {
        var twitchState = new TwitchState();
        if (File.Exists(TwitchFile))
        {
            await twitchState.LoadTwitchCredentials();
        }

        return twitchState;
    }

    public async Task<bool> Login()
    {
        if (TwitchApi is not null)
        {
            return true;
        }

        var scopesJoined = string.Join(" ", Scopes);
        var response = await AuthUrl
            .AllowAnyHttpStatus()
            .PostMultipartAsync(mp => mp
                .AddString("client_id", ClientId)
                .AddString("scopes", scopesJoined));

        if ((HttpStatusCode)response.StatusCode is not HttpStatusCode.OK)
        {
            // TODO: error
            return false;
        }

        var deviceResponse = await response.GetJsonAsync<DeviceFlowResponse>();

        Process.Start(new ProcessStartInfo(deviceResponse.VerificationUri)
        {
            UseShellExecute = true,
            Verb = "open"
        });

        var expireSource = new CancellationTokenSource();
        var validateSource = new CancellationTokenSource();

        var tokenExpired = Task.Delay(TimeSpan.FromSeconds(deviceResponse.ExpiresIn), expireSource.Token);
        var validated = IsValidated();

        await Task.WhenAny(tokenExpired, validated);

        var success = validated.IsCompletedSuccessfully;
        await validateSource.CancelAsync();
        await expireSource.CancelAsync();

        return success;

        async Task IsValidated()
        {
            while (!validateSource.Token.IsCancellationRequested)
            {
                await Task.Delay(TimeSpan.FromSeconds(2), validateSource.Token);

                var validateResponse = await ValidationUrl
                        .AllowAnyHttpStatus()
                        .PostMultipartAsync(mp => mp
                            .AddString("client_id", ClientId)
                            .AddString("scopes", scopesJoined)
                            .AddString("device_code", deviceResponse.DeviceCode)
                            .AddString("grant_type", "urn:ietf:params:oauth:grant-type:device_code"), cancellationToken: validateSource.Token)
                    ;

                if ((HttpStatusCode)validateResponse.StatusCode is HttpStatusCode.BadRequest)
                {
                    continue;
                }

                var tokenResponse = await validateResponse.GetJsonAsync<GetTokenResponse>();
                AccessToken = tokenResponse.AccessToken;
                RefreshToken = tokenResponse.RefreshToken;

                TwitchApi = new TwitchAPI
                {
                    Settings =
                    {
                        ClientId = ClientId,
                        AccessToken = AccessToken
                    }
                };

                await PostLoginInit();
                return;
            }
        }
    }

    public bool ConnectToTwitch(Action<TwitchClient> hooks)
    {
        if (User is null)
        {
            return false;
        }

        Client?.Disconnect();

        var clientOptions = new ClientOptions
        {
            MessagesAllowedInPeriod = 1000,
            ThrottlingPeriod = TimeSpan.FromSeconds(30)
        };

        var websocket = new WebSocketClient(clientOptions);
        Client = new TwitchClient(websocket);

        hooks(Client);

        var credentials = new ConnectionCredentials(User.Login, AccessToken);
        Client.Initialize(credentials, User.Login.ToLower());
        return Client.Connect();
    }

    private async Task LoadTwitchCredentials()
    {
        var hid = HwId.Generate();
        if (hid is null)
        {
            return;
        }

        var dataRaw = await File.ReadAllTextAsync(TwitchFile);
        if (dataRaw.IndexOf(KeySplitter, StringComparison.Ordinal) is -1)
        {
            return;
        }

        var data = dataRaw.Split(KeySplitter);
        if (data.Length != 2)
        {
            return;
        }

        AccessToken = StringCipher.Decrypt(data[0], hid);
        RefreshToken = StringCipher.Decrypt(data[1], hid);

        TwitchApi = new TwitchAPI
        {
            Settings =
            {
                ClientId = ClientId,
                AccessToken = AccessToken
            }
        };

        var validateResponse = await TwitchApi.Auth.ValidateAccessTokenAsync(AccessToken);
        if (validateResponse is not null)
        {
            await PostLoginInit();
            return;
        }

        // Token has expired, try to refresh
        var refresh = await ValidationUrl
            .AllowAnyHttpStatus()
            .PostMultipartAsync(mp => mp
                .AddString("client_id", ClientId)
                .AddString("grant_type", "refresh_token")
                .AddString("refresh_token", RefreshToken));

        if ((HttpStatusCode)refresh.StatusCode is not HttpStatusCode.OK)
        {
            AccessToken = "";
            RefreshToken = "";
            TwitchApi = null;
            return;
        }

        var tokenResponse = await refresh.GetJsonAsync<GetTokenResponse>();
        AccessToken = tokenResponse.AccessToken;
        RefreshToken = tokenResponse.RefreshToken;

        await PostLoginInit();
    }

    private async Task PostLoginInit()
    {
        if (TwitchApi is null)
        {
            return;
        }

        var users = await TwitchApi.Helix.Users.GetUsersAsync();
        User = users.Users[0]; // There should always be only one user connected
        Channel = new ChannelDetails
        {
            Information = (await TwitchApi.Helix.Channels.GetChannelInformationAsync(User.Id)).Data[0]
        };

        var hid = HwId.Generate();
        if (hid is null)
        {
            return;
        }

        await File.WriteAllTextAsync(TwitchFile, $"{StringCipher.Encrypt(AccessToken, hid)}{KeySplitter}{StringCipher.Encrypt(RefreshToken, hid)}");
    }
}