namespace ChaosMod.VotingProxy.VotingReceiver;

/// <summary>
///     Defines the interface that a voting receiver needs to satisfy
/// </summary>
internal interface IVotingReceiver
{
    /// <summary>
    ///     Events which get invoked when the voting receiver receives a message
    /// </summary>
    event EventHandler<MessageArgs> Message;
    /// <summary>
    ///     Initializes the voting receiver
    /// </summary>
    /// <returns></returns>
    Task<bool> Init();
    /// <summary>
    ///     Sends a message to the connected service
    /// </summary>
    /// <param name="message">Message that should be sent</param>
    Task SendMessage(string message);
}