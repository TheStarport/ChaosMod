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
internal class TwitchVotingReceiver(ChaosModConfig config, ChaosCommunicator communicator, TwitchState twitch) : IVotingReceiver
{
    private const int ReconnectInterval = 1000;

    public event EventHandler<MessageArgs>? Message;

    private readonly ILogger _logger = Log.Logger.ForContext<TwitchVotingReceiver>();

    private readonly ChaosCommunicator _communicator = communicator;

    public async Task<bool> Init()
    {
        if (!twitch.Validated && !await twitch.Login())
        {
            _logger.Error("Unable to login to Twitch. Terminating.");
            return false;
        }

        if (!twitch.ConnectToTwitch(client =>
            {
                client.OnDisconnected += OnDisconnected;
                client.OnConnected += OnConnected;
                client.OnError += OnError;
                client.OnIncorrectLogin += OnIncorrectLogin;
                client.OnFailureToReceiveJoinConfirmation += OnFailureToReceiveJoinConfirmation;
                client.OnJoinedChannel += OnJoinedChannel;
                client.OnMessageReceived += OnMessageReceived;
            }))
        {
            _logger.Error("Twitch connection failed");
            return false;
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
                twitch.Client.SendMessage(twitch.Client.JoinedChannels[0].Channel, msg);
            }
        }
        catch (Exception e)
        {
            _logger.Error(e, $"Failed to send message \"{message}\" to channel \"{twitch.Channel!.Information.BroadcasterLogin}\"");
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
        _communicator.SendErrorMessage("Invalid Twitch credentials. Please verify your config.");
        twitch.Client?.Disconnect();
    }

    /// <summary>
    ///     Called when the twitch client has not received a join confirmation (callback)
    /// </summary>
    private void OnFailureToReceiveJoinConfirmation(object? sender, OnFailureToReceiveJoinConfirmationArgs e)
    {
        _logger.Error("Invalid Twitch channel, please check specified Twitch channel.");
        _communicator.SendErrorMessage("Invalid Twitch channel. Please verify your config.");
        _communicator.Dispose();
        twitch.Client?.Disconnect();
    }

    /// <summary>
    ///     Called when the twitch client joins a channel (callback)
    /// </summary>
    private void OnJoinedChannel(object? sender, OnJoinedChannelArgs e)
    {
        _logger.Information($"Successfully joined Twitch channel \"{twitch.Channel!.Information.BroadcasterName}\"");
    }

    /// <summary>
    ///     Called when the twitch client receives a message
    /// </summary>
    private void OnMessageReceived(object? sender, OnMessageReceivedArgs e)
    {
        var chatMessage = e.ChatMessage;

        _logger.Debug("Twitch Message - {username}: {chatMessage}", chatMessage.Username, chatMessage.Message);

        var @event = new MessageArgs
        {
            Message = chatMessage.Message.Trim(),
            ClientId = chatMessage.UserId,
            Username = chatMessage.Username.ToLower() // lower case the username to allow case-insensitive comparisons
        };

        Message?.Invoke(this, @event);
    }
}