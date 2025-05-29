using System.Text.Json.Serialization;

namespace ChaosMod.Common.Models;

[Serializable]
public class GetTokenResponse
{
    [JsonPropertyName("access_token")] public required string AccessToken { get; set; }
    [JsonPropertyName("expires_in")] public int ExpiresIn { get; set; }
    [JsonPropertyName("scope")] public required IList<string> Scope { get; set; }
    [JsonPropertyName("refresh_token")] public required string RefreshToken { get; set; }
    [JsonPropertyName("token_type")] public required string TokenType { get; set; }
}