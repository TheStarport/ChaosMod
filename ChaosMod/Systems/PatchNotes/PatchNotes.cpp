#include "PCH.hpp"

#include "PatchNotes.hpp"

#include "Systems/ImguiComponents/ImGuiManager.hpp"

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <neargye/semver.hpp>

const std::vector<std::string> ReleaseNouns = {
#include "Nouns.txt"
};

const std::vector<std::string> ReleaseAdjectives = {
#include "Adjectives.txt"
};

void PatchNotes::LoadPatches()
{
    std::string path;
    path.resize(MAX_PATH, '\0');
    GetUserDataPath(path.data());
    path.erase(std::ranges::find(path, '\0'), path.end());

    Log(std::format("Attempting to load chaos.json. {}", path + "\\patches.json"));

    std::ifstream file(path + "\\patches.json", std::ios_base::binary | std::ios_base::in);

    // If the file exists, load it.
    if (file.is_open())
    {
        using StreamIter = std::istreambuf_iterator<char>;
        std::string content(StreamIter{ file }, StreamIter{});
        if (file)
        {
            try
            {
                ResetPatches(false, true);

                const auto json = nlohmann::json::parse(content);
                ASSERT(json.is_array(), "Patch notes must be an array.");

                for (const auto& obj : json)
                {
                    ASSERT(obj.is_object(), "Every array item must be an object.");

                    const auto patch = std::make_shared<Patch>();
                    patch->date = obj["date"];
                    patch->version = obj["version"];
                    auto _ = semver::from_string(patch->version); // Not used, excepts if invalid

                    for (auto& changes = obj["changes"]; const auto& changeObj : changes)
                    {
                        ASSERT(changes.is_object(), "Changes must be objects");

                        auto change = GetChangePtr(changeObj["type"].get<ChangeType>());

                        change->FromJson(changeObj);
                        patch->changes.emplace_back(change);
                    }

                    availablePatches.emplace_back(patch);
                }
            }
            catch (std::exception& ex)
            {
                Log(std::format("Failed to parse patch notes.json {}. Attempting to reset file.", ex.what()));
            }
        }
    }
}

void PatchNotes::SavePatches()
{
    using namespace nlohmann;
    auto json = json::array();
    for (const auto& patch : availablePatches)
    {
        auto obj = json::object();
        obj["date"] = patch->date;
        auto changes = json::array();

        for (const auto& change : patch->changes)
        {
            changes.push_back(change->ToJson());
        }

        obj["changes"] = changes;
        json.push_back(obj);
    }

    std::string path;
    path.resize(MAX_PATH, '\0');
    GetUserDataPath(path.data());
    path.erase(std::ranges::find(path, '\0'), path.end());

    std::ofstream file(path + "\\patches.json");

    const std::string jsonStr = json.dump(4);

    file << jsonStr << std::endl;
    file.close();

    Log(std::format("Saving patch notes: {}", path + "\\patches.json"));
}

void PatchNotes::RevertPatch(const std::shared_ptr<Patch>& patch)
{
    patchNotes.pop_back();

    for (const auto& change : patch->changes)
    {
        change->Revert();
    }

    appliedPatches.pop();
}

std::vector<PatchNotes::StringPatch>& PatchNotes::GetPatchNotes() { return patchNotes; }

void PatchNotes::ResetPatches(const bool reapply, bool clean)
{
    while (!appliedPatches.empty())
    {
        const auto patch = appliedPatches.top();
        RevertPatch(patch);
    }

    if (clean)
    {
        availablePatches.clear();
        patchNotes.clear();
        SavePatches();
        return;
    }

    if (reapply)
    {
        for (const auto& patch : availablePatches)
        {
            ApplyPatch(patch);
        }
    }
}

void PatchNotes::ApplyPatch(const std::shared_ptr<Patch>& patch)
{
    appliedPatches.push(patch);

    std::vector<std::string> notes;
    for (const auto& change : patch->changes)
    {
        notes.emplace_back(change->description);
        change->Apply();
    }

    std::chrono::sys_time time{ std::chrono::seconds{ patch->date } };
    patchNotes.emplace_back(patch->version, std::format("{0:%Y-%m-%d %H:%M:%S}", time), notes, patch->releaseName);

    ImGuiManager::ShowPatchNotes();
}

void PatchNotes::GeneratePatch()
{
    uint changeCount;

    auto patch = std::make_shared<Patch>();
    semver::version lastVersion{ "1.0.0" };
    if (!availablePatches.empty())
    {
        const auto& lastPatch = availablePatches.back();
        lastVersion = semver::from_string(lastPatch->version);
    }

    const auto versionIncrement = Random::i()->Uniform(1u, 20u);
    if (versionIncrement < 15 && lastVersion.patch != 255)
    {
        lastVersion.patch++;
        changeCount = Random::i()->Uniform(5u, 10u);
    }
    else if (versionIncrement < 20 && lastVersion.minor != 255)
    {
        lastVersion.minor++;
        lastVersion.patch = 0;
        changeCount = Random::i()->Uniform(15u, 25u);
    }
    else
    {
        lastVersion.patch = 0;
        lastVersion.minor = 0;
        lastVersion.major++;
        changeCount = 50;
        patch->releaseName = std::format(
            "{} {}", ReleaseAdjectives[Random::i()->Uniform(0u, ReleaseAdjectives.size())], ReleaseNouns[Random::i()->Uniform(0u, ReleaseNouns.size())]);
    }

    patch->version = lastVersion.to_string();
    patch->date = TimeUtils::UnixTime<std::chrono::seconds>();

    for (auto i = 0u; i < changeCount; ++i)
    {
        auto type = GetRandomChangeType();
        const auto change = GetChangePtr(static_cast<ChangeType>(type));

        change->Generate();
        patch->changes.emplace_back(change);
    }

    availablePatches.emplace_back(patch);
    ApplyPatch(patch);
    SavePatches();
}

ChangeType PatchNotes::GetRandomChangeType()
{
    static const std::array effectTypeDistribution = {
        EquipmentChange<ChangeType::Gun>::GetEffectCount(),      EquipmentChange<ChangeType::Ship>::GetEffectCount(),
        EquipmentChange<ChangeType::Cm>::GetEffectCount(),       EquipmentChange<ChangeType::CmAmmo>::GetEffectCount(),
        EquipmentChange<ChangeType::Mine>::GetEffectCount(),     EquipmentChange<ChangeType::MineAmmo>::GetEffectCount(),
        EquipmentChange<ChangeType::Thruster>::GetEffectCount(), EquipmentChange<ChangeType::Shield>::GetEffectCount(),
        EquipmentChange<ChangeType::GunAmmo>::GetEffectCount()
    };

    return static_cast<ChangeType>(Random::i()->Weighted(effectTypeDistribution.begin(), effectTypeDistribution.end()) + 1);
}

std::shared_ptr<Change> PatchNotes::GetChangePtr(const ChangeType type)
{
    std::shared_ptr<Change> change;

    // clang-format off
    switch (type)
    {
        case ChangeType::Gun: 
            change = std::make_shared<EquipmentChange<ChangeType::Gun>>();
            break;
        case ChangeType::Ship: 
            change = std::make_shared<EquipmentChange<ChangeType::Ship>>();
            break;
        case ChangeType::Cm: 
            change = std::make_shared<EquipmentChange<ChangeType::Cm>>();
            break;
        case ChangeType::CmAmmo: 
            change = std::make_shared<EquipmentChange<ChangeType::CmAmmo>>();
            break;
        case ChangeType::Mine: 
            change = std::make_shared<EquipmentChange<ChangeType::Mine>>();
            break;
        case ChangeType::MineAmmo: 
            change = std::make_shared<EquipmentChange<ChangeType::MineAmmo>>();
            break;
        case ChangeType::Thruster: 
            change = std::make_shared<EquipmentChange<ChangeType::Thruster>>();
            break;
        case ChangeType::Shield: 
            change = std::make_shared<EquipmentChange<ChangeType::Shield>>();
            break;
        case ChangeType::GunAmmo: 
            change = std::make_shared<EquipmentChange<ChangeType::GunAmmo>>();
            break;
        default:   // NOLINT(clang-diagnostic-covered-switch-default)
            ASSERT(false, "Invalid type provided in change log.");
    }
    // clang-format on

    return change;
}
