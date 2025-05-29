using System.Text.Json.Serialization;

namespace ChaosMod.Common.Models;

[Serializable]
public class DeviceFlowResponse
{
    [JsonPropertyName("device_code")]
    public required string DeviceCode { get; set; }

    [JsonPropertyName("verification_uri")]
    public required string VerificationUri { get; set; }

    [JsonPropertyName("user_code")]
    public required string UserCode { get; set; }

    [JsonPropertyName("interval")]
    public required int Interval { get; set; }

    [JsonPropertyName("expires_in")]
    public required int ExpiresIn { get; set; }
}