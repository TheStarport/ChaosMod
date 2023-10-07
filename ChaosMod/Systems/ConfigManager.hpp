#pragma once

#include <nlohmann/json.hpp>

class ConfigManager : public Singleton<ConfigManager>
{
    public:
        float timeBetweenChaos = 30.0f;
        float defaultEffectDuration = 90.0f;
        bool allowMetaEffects = true;
        uint totalAllowedConcurrentEffects = 6;
        bool enableTwitchVoting = false;
        float baseTwitchVoteWeight = 1.0f;
        DWORD progressBarColor = 0xFFFFFFFF;
        DWORD progressBarTextColor = 0xFF00FF00;
        bool showTimeRemainingOnEffects = false;
        uint timeBetweenSavesInSeconds = 300;
        bool allowAutoSavesDuringCombat = false;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigManager, timeBetweenChaos, defaultEffectDuration, allowMetaEffects, totalAllowedConcurrentEffects,
                                       enableTwitchVoting, baseTwitchVoteWeight, progressBarColor, progressBarTextColor, showTimeRemainingOnEffects,
                                       timeBetweenSavesInSeconds, allowAutoSavesDuringCombat);

        void Save();
        static ConfigManager* Load();
};
