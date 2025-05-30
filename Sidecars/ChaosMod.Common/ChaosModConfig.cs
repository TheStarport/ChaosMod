using System.Text.Json;
using System.Text.Json.Serialization;

namespace ChaosMod.Common;

[Serializable]
public class PatchNoteSettings
{
    public int ChangesPerMajorMin = 40;
    public int ChangesPerMinorMin = 15;
    public int ChangesPerPatchMin = 7;
    public bool CountDownWhileOnBases = true;
    public bool DisplayInColor = true;
    public bool Enable;
    public float TimeBetweenPatchesInMinutes = 10;
}

public enum ProgressBar
{
    TopBar,
    SideBar,
    Clock,
    Countdown
}

public enum TwitchVoteVisibility
{
    NotVisible,
    TotalOnly,
    PerEffectTotal,
    PerEffectPercentage
}

public enum VotingMode
{
    Majority,
    Percentage
}

[Serializable]
public class ChaosSettings
{
    public float BaseTwitchVoteWeight = 1;
    public bool BlockTeleportsDuringMissions;
    public float DefaultEffectDuration = 135;
    public bool Enable;
    public bool EnableTwitchVoting;
    public uint ProgressBarColor = 0xFFFFFFFF;
    public uint ProgressBarTextColor = 0xFF00FF00;
    public ProgressBar ProgressBarType = ProgressBar.Countdown;
    public bool ShowTimeRemainingOnEffects;
    public float TimeBetweenChaos = 45;
    public Dictionary<string, Dictionary<string, bool>> ToggledEffects = [];
    public int TotalAllowedConcurrentEffects = 8;

    // Twitch Settings

    public TwitchVoteVisibility TwitchVoteVisibility = TwitchVoteVisibility.PerEffectPercentage;
    public VotingMode VotingMode = VotingMode.Majority;
    public string VotingPrefix = string.Empty;
    public bool SendMessageUpdates;
}

[Serializable]
public class AutoSaveSettings
{
    public bool AllowAutoSavesDuringCombat = false;
    public bool Enable;
    public int TimeBetweenSavesInSeconds = 300;
}

public enum TimerType
{
    TimePlaying,
    TimeUntilChaos,
    TimeUntilPatchNote
}

[Serializable]
public class DiscordSettings
{
    public bool Enable;
    public TimerType TimerType = TimerType.TimePlaying;
}

[Serializable]
[JsonUnmappedMemberHandling(JsonUnmappedMemberHandling.Disallow)]
public class ChaosModConfig
{
    public required AutoSaveSettings AutoSaveSettings;
    public required ChaosSettings ChaosSettings;
    public required DiscordSettings DiscordSettings;
    public required bool FirstTime = true;
    public required PatchNoteSettings PatchNotes;

    [JsonIgnore]
    private static readonly string _filePath =
        Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "My Games", "FLChaosMod", "chaos.json");

    [JsonIgnore]
    private static readonly JsonSerializerOptions _serializerSettings = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        AllowTrailingCommas = true,
        PreferredObjectCreationHandling = JsonObjectCreationHandling.Populate,
        DictionaryKeyPolicy = JsonNamingPolicy.CamelCase,
        IncludeFields = true,
        WriteIndented = true
    };

    public void Save()
    {
        File.WriteAllText(_filePath, JsonSerializer.Serialize(this, _serializerSettings));
    }

    public static ChaosModConfig? Load(bool reset)
    {
        if (!File.Exists(_filePath) || reset)
        {
            // We use Activator here to bypass the 'required' flags which are for serialization only
            var config = new ChaosModConfig
            {
                AutoSaveSettings = Activator.CreateInstance<AutoSaveSettings>(),
                ChaosSettings = Activator.CreateInstance<ChaosSettings>(),
                DiscordSettings = Activator.CreateInstance<DiscordSettings>(),
                FirstTime = true,
                PatchNotes = Activator.CreateInstance<PatchNoteSettings>()
            };

            config.Save();
            return config;
        }

        try
        {
            return JsonSerializer.Deserialize<ChaosModConfig>(File.ReadAllText(_filePath), _serializerSettings);
        }
        catch (JsonException ex)
        {
            return null;
        }
    }
}