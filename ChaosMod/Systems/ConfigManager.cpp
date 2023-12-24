#include "PCH.hpp"

#include "ConfigManager.hpp"

#include "Effects/ActiveEffect.hpp"

#include <magic_enum.hpp>

void ConfigManager::Save()
{
    std::string path;
    path.resize(MAX_PATH, '\0');
    GetUserDataPath(path.data());
    path.erase(std::ranges::find(path, '\0'), path.end());

    std::ofstream file(path + "\\chaos.json");

    const nlohmann::json json = *this;
    const std::string jsonStr = json.dump(4);

    file << jsonStr << std::endl;
    file.close();

    Log(std::format("Saving json config: {}", path + "\\chaos.json"));
}

ConfigManager* ConfigManager::Load()
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
                [[maybe_unused]] auto manager = std::make_unique<ConfigManager>(nlohmann::json::parse(content));
                return i(&manager);
            }
            catch (std::exception& ex)
            {
                Log(std::format("Failed to parse chaos.json {}. Attempting to reset file.", ex.what()));
            }
        }
    }

    auto instance = i();
    if (instance->toggledEffects.empty())
    {
        for (const auto effects = ActiveEffect::GetAllEffects(); const auto& effect : effects)
        {
            auto& info = effect->GetEffectInfo();
            auto name = std::string(magic_enum::enum_name<EffectType>(info.category));
            if (!instance->toggledEffects.contains(name))
            {
                instance->toggledEffects[name] = {};
            }

            instance->toggledEffects[name][info.effectName] = true;
        }
    }

    instance->Save();
    return instance;
}
