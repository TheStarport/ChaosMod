using ChaosMod.Common;
using ChaosMod.Common.Twitch;
using ChaosMod.VotingProxy.ChaosPipe;
using Serilog;
using TwitchLib.Client.Events;
using TwitchLib.Communication.Events;

namespace ChaosMod.VotingProxy.VotingReceiver;

/// <summary>
///     Twitch voting receiver implementation
/// </summary>
internal class TwitchVotingReceiver(ChaosModConfig config, ChaosPipeClient chaosPipe, TwitchState twitch) : IVotingReceiver
{
    private const int ReconnectInterval = 1000;

    public event EventHandler<MessageArgs>? Message;

    private readonly ILogger _logger = Log.Logger.ForContext<TwitchVotingReceiver>();

    private bool _isReady;

    public async Task<bool> Init()
    {
        _logger.Information("Initializing Twitch Voting Receiver");

        if (!twitch.ConnectToTwitch())
        {
            _logger.Error("Twitch connection failed");
            return false;
        }

        twitch.Client!.OnDisconnected += OnDisconnected;
        twitch.Client.OnConnected += OnConnected;
        twitch.Client.OnError += OnError;
        twitch.Client.OnIncorrectLogin += OnIncorrectLogin;
        twitch.Client.OnFailureToReceiveJoinConfirmation += OnFailureToReceiveJoinConfirmation;
        twitch.Client.OnJoinedChannel += OnJoinedChannel;
        twitch.Client.OnMessageReceived += OnMessageReceived;

        if (!twitch.Client.Connect())
        {
            _logger.Error("Failed to connect to channel");
            return false;
        }

        while (!_isReady)
        {
            await Task.Delay(100);
        }

        return true;
    }

    private void OnDisconnected(object? sender, OnDisconnectedEventArgs e)
    {
        _logger.Error("Disconnected from the twitch channel, trying to reconnect");
        Thread.Sleep(ReconnectInterval);

        twitch.Client?.Connect();
    }

    public Task SendMessage(string message)
    {
        if (twitch.Client is null || !config.ChaosSettings.SendMessageUpdates)
        {
            return Task.CompletedTask;
        }

        try
        {
            foreach (var msg in message.Split("\n"))
            {
                twitch.Client.SendMessage(twitch.Channel!.Information.BroadcasterName, msg);
            }
        }
        catch (Exception e)
        {
            _logger.Error(e, $"Failed to send message \"{message}\" to channel \"{twitch.Channel!.Information.BroadcasterName}\"");
        }

        return Task.CompletedTask;
    }

    /// <summary>
    ///     Called when the twitch client connects (callback)
    /// </summary>
    private void OnConnected(object? sender, OnConnectedArgs e)
    {
        _logger.Information("Successfully connected to Twitch");
    }

    /// <summary>
    ///     Called when the twitch clients errors (callback)
    /// </summary>
    private void OnError(object? sender, OnErrorEventArgs e)
    {
        _logger.Error(e.Exception, "Client error, disconnecting");
        twitch.Client?.Disconnect();
    }

    /// <summary>
    ///     Called when the twitch client has an failed login attempt (callback)
    /// </summary>
    private void OnIncorrectLogin(object? sender, OnIncorrectLoginArgs e)
    {
        _logger.Error("Invalid Twitch login credentials, check user name and oauth.");
        chaosPipe.SendErrorMessage("Invalid Twitch credentials. Please verify your config.");
        twitch.Client?.Disconnect();
    }

    /// <summary>
    ///     Called when the twitch client has not received a join confirmation (callback)
    /// </summary>
    private void OnFailureToReceiveJoinConfirmation(object? sender, OnFailureToReceiveJoinConfirmationArgs e)
    {
        _logger.Error("Invalid Twitch channel, please check specified Twitch channel.");
        chaosPipe.SendErrorMessage("Invalid Twitch channel. Please verify your config.");
        twitch.Client?.Disconnect();
    }

    /// <summary>
    ///     Called when the twitch client joins a channel (callback)
    /// </summary>
    private void OnJoinedChannel(object? sender, OnJoinedChannelArgs e)
    {
        _logger.Information($"Successfully joined Twitch channel \"{twitch.Channel!.Information.BroadcasterName}\"");
        _isReady = true;
    }

    /// <summary>
    ///     Called when the twitch client receives a message
    /// </summary>
    private void OnMessageReceived(object? sender, OnMessageReceivedArgs e)
    {
        var chatMessage = e.ChatMessage;

        var @event = new MessageArgs
        {
            Message = chatMessage.Message.Trim(),
            ClientId = chatMessage.UserId,
            Username = chatMessage.Username.ToLower() // lower case the username to allow case-insensitive comparisons
        };
        Message?.Invoke(this, @event);
    }
}