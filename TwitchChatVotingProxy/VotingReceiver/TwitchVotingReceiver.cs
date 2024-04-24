using Serilog;
using TwitchChatVotingProxy.ChaosPipe;
using TwitchLib.Client;
using TwitchLib.Client.Events;
using TwitchLib.Client.Models;
using TwitchLib.Communication.Clients;
using TwitchLib.Communication.Events;

namespace TwitchChatVotingProxy.VotingReceiver;

/// <summary>
/// Twitch voting receiver implementation
/// </summary>
internal class TwitchVotingReceiver : IVotingReceiver
{
    private const int ReconnectInterval = 1000;

    public event EventHandler<MessageArgs>? Message;

    private readonly string? _channelName;
    private readonly string? _oAuth;
    private readonly string? _userName;

    private TwitchClient? _client;
    private readonly ChaosPipeClient _chaosPipe;
    private readonly ILogger _logger = Log.Logger.ForContext<TwitchVotingReceiver>();

    private bool _isReady;

    public TwitchVotingReceiver(Configuration config, ChaosPipeClient chaosPipe)
    {
        _channelName = config.TwitchChannelName;
        _oAuth = config.TwitchToken;
        _userName = config.TwitchUsername;

        _chaosPipe = chaosPipe;
    }

    public async Task<bool> Init()
    {
        if (string.IsNullOrWhiteSpace(_channelName))
        {
            _logger.Fatal("Twitch channel is not set!");
            _chaosPipe.SendErrorMessage("Twitch channel is not set. Please set one in the config utility.");

            return false;
        }
        
        if (string.IsNullOrWhiteSpace(_oAuth))
        {
            _logger.Fatal("Twitch OAuth token is not set!");
            _chaosPipe.SendErrorMessage("Twitch OAuth token is not set. Please set one in the config utility.");

            return false;
        }
        
        if (string.IsNullOrWhiteSpace(_userName))
        {
            _logger.Fatal("Twitch username is not set!");
            _chaosPipe.SendErrorMessage("Twitch username is not set. Please set one in the config utility.");

            return false;
        }

        _logger.Information($"Trying to connect to twitch channel \"{_channelName}\" with user \"{_userName}\"");

        _client = new TwitchClient(new WebSocketClient());
        _client.Initialize(new ConnectionCredentials(_userName, _oAuth), _channelName);

        _client.OnDisconnected += OnDisconnected;
        _client.OnConnected += OnConnected;
        _client.OnError += OnError;
        _client.OnIncorrectLogin += OnIncorrectLogin;
        _client.OnFailureToReceiveJoinConfirmation += OnFailureToReceiveJoinConfirmation;
        _client.OnJoinedChannel += OnJoinedChannel;
        _client.OnMessageReceived += OnMessageReceived;

        if (!_client.Connect())
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
        
        _client?.Connect();
    }

    public Task SendMessage(string message)
    {
        if (_client is null)
        {
            return Task.FromResult(0);
        }

        try
        {
            foreach (var msg in message.Split("\n"))
            {
                _client.SendMessage(_channelName, msg);
            }
        }
        catch (Exception e)
        {
            _logger.Error(e, $"Failed to send message \"{message}\" to channel \"{_channelName}\"");
        }

        return Task.FromResult(0);
    }

    /// <summary>
    /// Called when the twitch client connects (callback)
    /// </summary>
    private void OnConnected(object? sender, OnConnectedArgs e)
    {
        _logger.Information("Successfully connected to Twitch");
    }
    
    /// <summary>
    /// Called when the twitch clients errors (callback)
    /// </summary>
    private void OnError(object? sender, OnErrorEventArgs e)
    {
        _logger.Error(e.Exception, "Client error, disconnecting");
        _client?.Disconnect();
    }
    
    /// <summary>
    /// Called when the twitch client has an failed login attempt (callback)
    /// </summary>
    private void OnIncorrectLogin(object? sender, OnIncorrectLoginArgs e)
    {
        _logger.Error("Invalid Twitch login credentials, check user name and oauth.");
        _chaosPipe.SendErrorMessage("Invalid Twitch credentials. Please verify your config.");
        _client?.Disconnect();
    }
    
    /// <summary>
    /// Called when the twitch client has not received a join confirmation (callback)
    /// </summary>
    private void OnFailureToReceiveJoinConfirmation(object? sender, OnFailureToReceiveJoinConfirmationArgs e)
    {
        _logger.Error("Invalid Twitch channel, please check specified Twitch channel.");
        _chaosPipe.SendErrorMessage("Invalid Twitch channel. Please verify your config.");
        _client?.Disconnect();
    }
    
    /// <summary>
    /// Called when the twitch client joins a channel (callback)
    /// </summary>
    private void OnJoinedChannel(object? sender, OnJoinedChannelArgs e)
    {
        _logger.Information($"Successfully joined Twitch channel \"{_channelName}\"");
        _isReady = true;
    }
    
    /// <summary>
    /// Called when the twitch client receives a message
    /// </summary>
    private void OnMessageReceived(object? sender, OnMessageReceivedArgs e)
    {
        var chatMessage = e.ChatMessage;

        var evnt = new MessageArgs
        {
            Message = chatMessage.Message.Trim(),
            ClientId = chatMessage.UserId,
            Username = chatMessage.Username.ToLower() // lower case the username to allow case-insensitive comparisons
        };
        Message?.Invoke(this, evnt);
    }
}