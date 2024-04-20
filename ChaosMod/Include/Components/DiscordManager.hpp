#pragma once

#include "Component.hpp"

class DiscordManager final : public Component
{
    bool enabled = false;
    float timer = 30.f;

    static std::string GetStatus();

    public:
        DiscordManager();
        void SetActivity(std::string_view description, uint startTime = 0, uint endTime = 0) const;
        void Update();
};