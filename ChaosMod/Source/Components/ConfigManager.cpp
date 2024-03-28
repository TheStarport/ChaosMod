#include "PCH.hpp"

#include "Components/ConfigManager.hpp"

#include "Effects/ActiveEffect.hpp"

#include <magic_enum.hpp>

void ConfigManager::Save(std::string_view path)
{
    if (path.empty())
    {
        static std::string userPath;
        userPath.resize(MAX_PATH, '\0');
        GetUserDataPath(userPath.data());
        userPath.erase(std::ranges::find(userPath, '\0'), userPath.end());
        userPath += "\\chaos.json";
        path = userPath;
    }

    std::string fileLoc = std::format("{}", path);
    std::ofstream file(fileLoc);

    const nlohmann::json json = *this;
    const std::string jsonStr = json.dump(4);

    file << jsonStr << std::endl;
    file.close();

    Log(std::format("Saving json config: {}", fileLoc));
}

std::shared_ptr<ConfigManager> ConfigManager::Load()
{
    std::string path;
    path.resize(MAX_PATH, '\0');
    GetUserDataPath(path.data());
    path.erase(std::ranges::find(path, '\0'), path.end());

    Log(std::format("Attempting to load chaos.json. {}", path + "\\chaos.json"));

    std::ifstream file(path + "\\chaos.json", std::ios_base::binary | std::ios_base::in);

    // If the file exists, load it.
    if (file.is_open())
    {
        using StreamIter = std::istreambuf_iterator<char>;
        std::string content(StreamIter{ file }, StreamIter{});
        if (file)
        {
            try
            {
                [[maybe_unused]] const auto manager = std::make_shared<ConfigManager>(nlohmann::json::parse(content));
                ResetComponent<ConfigManager>(manager);
            }
            catch (std::exception& ex)
            {
                Log(std::format("Failed to parse chaos.json {}. Attempting to reset file.", ex.what()));
                SetComponent<ConfigManager>();
            }
        }
    }
    else
    {
        SetComponent<ConfigManager>();
    }

    auto instance = Get<ConfigManager>();

    // If new effects or new categories are added, enable them by default
    for (const auto effects = ActiveEffect::GetAllEffects(); const auto& effect : effects)
    {
        auto& info = effect->GetEffectInfo();
        auto name = std::string(magic_enum::enum_name<EffectType>(info.category));
        if (!instance->chaosSettings.toggledEffects.contains(name))
        {
            instance->chaosSettings.toggledEffects[name] = {};
        }

        if (!instance->chaosSettings.toggledEffects[name].contains(info.effectName))
        {
            instance->chaosSettings.toggledEffects[name][info.effectName] = true;
        }
    }

    instance->Save();
    return instance;
}
