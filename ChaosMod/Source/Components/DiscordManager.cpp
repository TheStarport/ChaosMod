#include "PCH.hpp"

#include "Components/DiscordManager.hpp"

#include "Components/ConfigManager.hpp"

#include "core.h"

using namespace discord;

Core* core{};
i64 timeStarted = 0;

std::string DiscordManager::GetStatus()
{
    uint base;
    pub::Player::GetBase(1, base);

    if (base)
    {
        return std::format("Currently on {}", ChaosMod::GetInfocardName(Universe::get_base(base)->baseIdS));
    }

    uint systemId;
    pub::Player::GetSystem(1, systemId);
    const auto system = Universe::get_system(systemId);

    // Not on a base and not in a system, therefore we assume main menu
    if (!system)
    {
        return "Waiting in the main menu";
    }

    return std::format("Flying around {}", ChaosMod::GetInfocardName(system->idsName));
}

DiscordManager::DiscordManager()
{
    timeStarted = static_cast<i64>(TimeUtils::UnixTime<std::chrono::seconds>());
    if (Result result = Core::Create(485009172170145803, DiscordCreateFlags_NoRequireDiscord, &core); result != Result::Ok)
    {
        Log(std::format("Unable to init Discord (code: {})", static_cast<int>(result)));
        return;
    }

    enabled = true;
}

void DiscordManager::SetActivity(const std::string_view description, const uint startTime, const uint endTime) const
{
    if (!enabled)
    {
        return;
    }

    Activity activity{};
    activity.SetType(ActivityType::Playing);
    activity.SetName("Freelancer: ChaosMod");
    activity.SetDetails(description.data());
    activity.GetAssets().SetLargeImage("game");

    activity.SetState(GetStatus().c_str());

    activity.GetTimestamps().SetStart(startTime);
    activity.GetTimestamps().SetEnd(endTime);

    core->ActivityManager().UpdateActivity(activity, [](Result _){});
}

void DiscordManager::Update()
{
    if (!enabled || !Get<ConfigManager>()->discordSettings.enable)
    {
        return;
    }

    constexpr float delta = 1.f / 60.f;
    timer -= delta;
    if (timer < 0.f && Get<ConfigManager>()->discordSettings.timerType == DiscordSettings::TimerType::TimePlaying)
    {
        timer = 30.f;
        SetActivity("Enjoying the Chaos", timeStarted);
    }

    core->RunCallbacks();
}
