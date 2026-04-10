#pragma once

#include <string>
#include <unordered_map>

struct PatchNoteSettings
{
        bool enable = false;
        bool countDownWhileOnBases = true;
        bool displayInColor = true;
        float timeBetweenPatchesInMinutes = 10.f;
        unsigned changesPerPatchMin = 7;
        unsigned changesPerMinorMin = 15;
        unsigned changesPerMajorMin = 40;
};

struct ChaosSettings
{
        enum class ProgressBar
        {
            TopBar,
            SideBar,
            Clock,
            Countdown
        };

        enum class TwitchVoteVisibility
        {
            NotVisible,
            TotalOnly,
            PerEffectTotal,
            PerEffectPercentage
        };

        enum class VotingMode
        {
            Majority,
            Percentage
        };

        bool enable = false;
        bool blockTeleportsDuringMissions = false;
        float timeBetweenChaos = 45.0f;
        float defaultEffectDuration = 135.0f;
        unsigned totalAllowedConcurrentEffects = 8;

        bool enableTwitchVoting = false;
        float baseTwitchVoteWeight = 1.0f;
        TwitchVoteVisibility twitchVoteVisibility = TwitchVoteVisibility::PerEffectPercentage;
        VotingMode votingMode = VotingMode::Majority;
        std::string votingPrefix = "";
        bool sendMessageUpdates = true;

        unsigned long progressBarColor = 0xFFFFFFFF;
        unsigned long progressBarTextColor = 0xFF00FF00;
        bool showTimeRemainingOnEffects = false;
        ProgressBar progressBarType = ProgressBar::Countdown;
        std::unordered_map<std::string, std::unordered_map<std::string, bool>> toggledEffects;
};

struct AutoSaveSettings
{
        bool enable = false;
        unsigned timeBetweenSavesInSeconds = 300;
        bool allowAutoSavesDuringCombat = false;
};

struct DiscordSettings
{
        enum class TimerType
        {
            TimePlaying,
            TimeUntilChaos,
            TimeUntilPatchNote
        };

        bool enable = false;
        TimerType timerType = TimerType::TimePlaying;
};

struct ChaosConfig
{
        bool firstTime = true;
        ChaosSettings chaosSettings{};
        AutoSaveSettings autoSaveSettings{};
        PatchNoteSettings patchNotes{};
        DiscordSettings discordSettings{};
};
