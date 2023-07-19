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

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigManager, timeBetweenChaos, defaultEffectDuration, allowMetaEffects, totalAllowedConcurrentEffects,
                                       enableTwitchVoting, baseTwitchVoteWeight);

        void Save();
        static ConfigManager* Load();
};
