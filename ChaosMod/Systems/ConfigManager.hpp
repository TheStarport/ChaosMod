#pragma once

#include <nlohmann/json.hpp>

class ConfigManager : public Singleton<ConfigManager>
{
    public:
        float timeBetweenChaos = 45.0f;
        float defaultEffectDuration = 135.0f;
        bool allowMetaEffects = true;
        uint totalAllowedConcurrentEffects = 8;
        bool enableTwitchVoting = false;
        float baseTwitchVoteWeight = 1.0f;
        DWORD progressBarColor = 0xFFFFFFFF;
        DWORD progressBarTextColor = 0xFF00FF00;
        bool showTimeRemainingOnEffects = false;
        uint timeBetweenSavesInSeconds = 300;
        bool allowAutoSavesDuringCombat = false;
        std::map<std::string, std::map<std::string, bool>> toggledEffects;
        bool enablePatchNotes = true;
        bool countDownWhileOnBases = true;
        float timeBetweenPatchesInMinutes = 10.f;
        uint changesPerPatchMin = 7;
        uint changesPerMinorMin = 15;
        uint changesPerMajorMin = 40;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigManager, timeBetweenChaos, defaultEffectDuration, allowMetaEffects, totalAllowedConcurrentEffects,
                                       enableTwitchVoting, baseTwitchVoteWeight, progressBarColor, progressBarTextColor, showTimeRemainingOnEffects,
                                       timeBetweenSavesInSeconds, allowAutoSavesDuringCombat, toggledEffects, enablePatchNotes, countDownWhileOnBases,
                                       timeBetweenPatchesInMinutes, changesPerPatchMin, changesPerMinorMin, changesPerMajorMin);

        void Save();
        static ConfigManager* Load();
};
