#pragma once

#include <nlohmann/json.hpp>

struct PatchNoteSettings
{
        bool enable = false;
        bool countDownWhileOnBases = true;
        float timeBetweenPatchesInMinutes = 10.f;
        uint changesPerPatchMin = 7;
        uint changesPerMinorMin = 15;
        uint changesPerMajorMin = 40;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(PatchNoteSettings, enable, countDownWhileOnBases, timeBetweenPatchesInMinutes, changesPerPatchMin, changesPerMinorMin,
                                       changesPerMajorMin);
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

        bool enable = false;
        bool blockTeleportsDuringMissions = false;
        float timeBetweenChaos = 45.0f;
        float defaultEffectDuration = 135.0f;
        uint totalAllowedConcurrentEffects = 8;
        bool enableTwitchVoting = false;
        float baseTwitchVoteWeight = 1.0f;
        DWORD progressBarColor = 0xFFFFFFFF;
        DWORD progressBarTextColor = 0xFF00FF00;
        bool showTimeRemainingOnEffects = false;
        ProgressBar progressBarType = ProgressBar::Clock;
        std::map<std::string, std::map<std::string, bool>> toggledEffects;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ChaosSettings, enable, blockTeleportsDuringMissions, timeBetweenChaos, defaultEffectDuration,
                                       totalAllowedConcurrentEffects, enableTwitchVoting, baseTwitchVoteWeight, progressBarColor, progressBarTextColor,
                                       showTimeRemainingOnEffects, toggledEffects, progressBarType);
};

struct AutoSaveSettings
{
        bool enable = false;
        uint timeBetweenSavesInSeconds = 300;
        bool allowAutoSavesDuringCombat = false;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AutoSaveSettings, enable, timeBetweenSavesInSeconds, allowAutoSavesDuringCombat);
};

class ConfigManager : public Component
{
    public:
        ConfigManager() = default;
        ConfigManager(const ConfigManager& configManager)
        {
            // Copy all the data, skipping the vtable
            memcpy_s(reinterpret_cast<PCHAR>(this) + 4, sizeof(ConfigManager) - 4, reinterpret_cast<LPCSTR>(&configManager) + 4, sizeof(ConfigManager) - 4);
        }

        bool firstTime = true;
        ChaosSettings chaosSettings{};
        AutoSaveSettings autoSaveSettings{};
        PatchNoteSettings patchNotes{};

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigManager, firstTime, chaosSettings, autoSaveSettings, patchNotes);

        void Save(std::string_view path = "");
        static std::shared_ptr<ConfigManager> Load();
};
