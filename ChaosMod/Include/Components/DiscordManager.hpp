#pragma once

#include "Component.hpp"

#include <string>

class DiscordManager final : public Component
{
    bool enabled = false;
    float timer = 30.f;

    static std::string GetStatus();

    public:
        DiscordManager();
        void SetActivity(std::string_view description, unsigned startTime = 0, unsigned endTime = 0) const;
        void Update();
};