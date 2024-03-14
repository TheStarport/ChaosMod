#pragma once

#include <nlohmann/json.hpp>

class ConfigManager : public Component
{
    public:
        ConfigManager() = default;
        ConfigManager(const ConfigManager& configManager)
        {
            // Copy all the data, skipping the vtable
            memcpy_s(reinterpret_cast<PCHAR>(this) + 4, sizeof(ConfigManager) - 4, reinterpret_cast<LPCSTR>(&configManager) + 4, sizeof(ConfigManager) - 4);
        }

        enum class ProgressBar
        {
            TopBar,
            SideBar,
            Clock,
            Countdown
        };

        float timeBetweenChaos = 45.0f;
        float defaultEffectDuration = 135.0f;
        uint totalAllowedConcurrentEffects = 8;
        bool enableTwitchVoting = false;
        float baseTwitchVoteWeight = 1.0f;
        DWORD progressBarColor = 0xFFFFFFFF;
        DWORD progressBarTextColor = 0xFF00FF00;
        bool showTimeRemainingOnEffects = false;
        uint timeBetweenSavesInSeconds = 300;
        bool allowAutoSavesDuringCombat = false;
        bool blockTeleportsDuringMissions = false;
        std::map<std::string, std::map<std::string, bool>> toggledEffects;
        bool enablePatchNotes = true;
        bool countDownWhileOnBases = true;
        float timeBetweenPatchesInMinutes = 10.f;
        uint changesPerPatchMin = 7;
        uint changesPerMinorMin = 15;
        uint changesPerMajorMin = 40;
        ProgressBar progressBarType = ProgressBar::Clock;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigManager, timeBetweenChaos, defaultEffectDuration, totalAllowedConcurrentEffects, enableTwitchVoting,
                                       baseTwitchVoteWeight, progressBarColor, progressBarTextColor, showTimeRemainingOnEffects, timeBetweenSavesInSeconds,
                                       allowAutoSavesDuringCombat, blockTeleportsDuringMissions, toggledEffects, enablePatchNotes, countDownWhileOnBases,
                                       timeBetweenPatchesInMinutes, changesPerPatchMin, changesPerMinorMin, changesPerMajorMin, progressBarType);

        void Save();
        static std::shared_ptr<ConfigManager> Load();
};
