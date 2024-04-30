#include "PCH.hpp"

#include "Components/ConfigManager.hpp"
#include "CoreComponents/PatchNotes.hpp"

#include "Components/DiscordManager.hpp"
#include "ImGui/ImGuiManager.hpp"

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <Memory/OffsetHelper.hpp>
#include <neargye/semver.hpp>

void PatchNotes::LoadPatches()
{
    patchTime = Get<ConfigManager>()->patchNotes.timeBetweenPatchesInMinutes * 60;

    std::string path;
    path.resize(MAX_PATH, '\0');
    GetUserDataPath(path.data());
    path.erase(std::ranges::find(path, '\0'), path.end());

    Log(std::format("Attempting to load patch notes: {}", path + "\\patches.json"));

    std::ifstream file(path + "\\patches.json", std::ios_base::binary | std::ios_base::in);

    // If the file exists, load it.
    if (!file.is_open())
    {
        return;
    }

    using StreamIter = std::istreambuf_iterator<char>;
    std::string content(StreamIter{ file }, StreamIter{});
    try
    {
        const auto json = nlohmann::json::parse(content);
        ASSERT(json.is_object(), "Patch notes must be an object.");
        auto patches = json["patches"];

        file.close();

        while (!appliedPatches.empty())
        {
            const auto patch = appliedPatches.top();
            RevertPatch(patch);
        }

        availablePatches.clear();
        patchNotes.clear();

        for (const auto& obj : patches)
        {
            ASSERT(obj.is_object(), "Every array item must be an object.");

            const auto patch = std::make_shared<Patch>();
            patch->date = obj["date"];
            patch->version = obj["version"];
            (void)semver::from_string(patch->version); // Not used, excepts if invalid
            if (obj.contains("release"))
            {
                patch->releaseName = obj["release"];
            }

            auto changes = obj.at("changes").get<nlohmann::json::array_t>();
            for (const auto& changeObj : changes)
            {
                ASSERT(changeObj.is_object(), "Changes must be objects");

                auto change = GetChangePtr(static_cast<ChangeType>(changeObj["type"].get<int>()));

                change->FromJson(changeObj);
                patch->changes.emplace_back(change);
            }

            availablePatches.emplace_back(patch);
        }

        auto seed = json["seed"].get<std::string>();
        std::istringstream iss(seed);
        randomEngine.Reseed(iss);
    }
    catch (std::exception& ex)
    {
        Log(std::format("Failed to parse patch notes.json {}. Attempting to reset file.", ex.what()));
        ResetPatches(false, true);
    }
}

void PatchNotes::SavePatches()
{
    using namespace nlohmann;
    auto array = json::array();
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
        obj["version"] = patch->version;
        obj["release"] = patch->releaseName;
        array.push_back(obj);
    }

    auto json = json::object();
    json["patches"] = array;

    std::stringstream seed;
    randomEngine.GetCurrentSeed(seed);
    json["seed"] = seed.str();

    std::string path;
    path.resize(MAX_PATH, '\0');
    GetUserDataPath(path.data());
    path.erase(std::ranges::find(path, '\0'), path.end());

    std::ofstream file(path + "\\patches.json");

    const std::string jsonStr = json.dump();

    file << jsonStr << std::endl; // NOLINT
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
            ApplyPatch(patch, false);
        }
    }
}

void PatchNotes::ApplyPatch(const std::shared_ptr<Patch>& patch, const bool showPatchNotes)
{
    appliedPatches.push(patch);

    std::vector<std::pair<std::string, Change::ChangePositivity>> notes;
    for (const auto& change : patch->changes)
    {
        notes.emplace_back(std::pair{ change->description, change->positivity });
        change->Apply();
    }

    std::chrono::sys_time time{ std::chrono::seconds{ patch->date } };
    patchNotes.emplace_back(patch->version, std::format("{0:%Y-%m-%d %H:%M:%S}", time), notes, patch->releaseName);

    if (showPatchNotes)
    {
        ImGuiManager::ShowPatchNotes();
    }
}

void PatchNotes::GeneratePatch(const PatchVersion version)
{
    uint changeCount;

    auto patch = std::make_shared<Patch>();
    semver::version lastVersion{ "1.0.0" };
    if (!availablePatches.empty())
    {
        const auto& lastPatch = availablePatches.back();
        lastVersion = semver::from_string(lastPatch->version);
    }

    uint versionIncrement;
    switch (version)
    {
        case PatchVersion::Patch: versionIncrement = 1; break;
        case PatchVersion::Minor: versionIncrement = 15; break;
        case PatchVersion::Major: versionIncrement = 20; break;
        case PatchVersion::Undetermined:
        default: versionIncrement = randomEngine.Uniform(1u, 20u); break;
    }

    if (versionIncrement < 15 && lastVersion.patch != 255)
    {
        lastVersion.patch++;
        changeCount = randomEngine.Uniform(Get<ConfigManager>()->patchNotes.changesPerPatchMin, Get<ConfigManager>()->patchNotes.changesPerPatchMin * 2);
    }
    else if (versionIncrement < 20 && lastVersion.minor != 255)
    {
        lastVersion.minor++;
        lastVersion.patch = 0;
        changeCount = randomEngine.Uniform(Get<ConfigManager>()->patchNotes.changesPerMinorMin, Get<ConfigManager>()->patchNotes.changesPerMinorMin * 2);
    }
    else
    {
        lastVersion.patch = 0;
        lastVersion.minor = 0;
        lastVersion.major++;
        changeCount = randomEngine.Uniform(Get<ConfigManager>()->patchNotes.changesPerMajorMin, Get<ConfigManager>()->patchNotes.changesPerMajorMin * 2);
        patch->releaseName = std::format("{} {}", randomEngine.RandomAdjective(), randomEngine.RandomNoun());
    }

    patch->version = lastVersion.to_string();
    patch->date = TimeUtils::UnixTime<std::chrono::seconds>();

    for (auto i = 0u; i < changeCount; ++i)
    {
        auto type = GetRandomChangeType();
        const auto change = GetChangePtr(static_cast<ChangeType>(type));

        change->Generate(randomEngine);
        patch->changes.emplace_back(change);
    }

    availablePatches.emplace_back(patch);
    ApplyPatch(patch);
    SavePatches();
}

void PatchNotes::DoubleDown()
{
    const auto lastPatch = availablePatches.back();
    for (const auto& change : lastPatch->changes)
    {
        change->Multiply(2.f);
    }

    // Reapply just this one patch
    RevertPatch(lastPatch);
    ApplyPatch(lastPatch);

    SavePatches();
    ImGuiManager::ShowPatchNotes();
}

void PatchNotes::RevertLastPatch()
{
    const auto lastPatch = availablePatches.back();
    RevertPatch(lastPatch);

    ImGuiManager::ShowPatchNotes();
}

ChangeType PatchNotes::GetRandomChangeType()
{
    // MUST MATCH ENUM ORDER
    static const std::array effectTypeDistribution = {
        EquipmentChange<ChangeType::Gun>::GetEffectCount(),      EquipmentChange<ChangeType::GunAmmo>::GetEffectCount(),
        EquipmentChange<ChangeType::GunMotor>::GetEffectCount(), EquipmentChange<ChangeType::GunExplosion>::GetEffectCount(),
        EquipmentChange<ChangeType::Shield>::GetEffectCount(),   EquipmentChange<ChangeType::Thruster>::GetEffectCount(),
        EquipmentChange<ChangeType::Mine>::GetEffectCount(),     EquipmentChange<ChangeType::MineAmmo>::GetEffectCount(),
        EquipmentChange<ChangeType::Cm>::GetEffectCount(),       EquipmentChange<ChangeType::CmAmmo>::GetEffectCount(),
        EquipmentChange<ChangeType::Ship>::GetEffectCount(),     static_cast<uint>(static_cast<float>(CurrencyChange::GetEffectCount()) * 0.5f),
    };
    // Half the chance of currency change cause of how many there are

    return static_cast<ChangeType>(randomEngine.Weighted(effectTypeDistribution.begin(), effectTypeDistribution.end()) + 1);
}

// Unlike the other reseed, this will one will reset the entire engine to a clean slate
void PatchNotes::Reseed(const std::string_view seed)
{
    ResetPatches(false, true);
    randomEngine.Reseed(seed);
}

void PatchNotes::Reseed(std::istream& seed) { randomEngine.Reseed(seed); }

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
        case ChangeType::GunExplosion:
            change = std::make_shared<EquipmentChange<ChangeType::GunExplosion>>();
            break;
        case ChangeType::GunMotor:
            change = std::make_shared<EquipmentChange<ChangeType::GunMotor>>();
            break;
        case ChangeType::Currency:
            change = std::make_shared<CurrencyChange>();
            break;
        default:   // NOLINT(clang-diagnostic-covered-switch-default)
            ASSERT(false, "Invalid type provided in change log.");
    }
    // clang-format on

    return change;
}

void PatchNotes::Update(const float delta)
{
    timeSinceLastUpdate -= delta;

    // Check that the game is not paused and patch notes are enabled
    if (Get<ConfigManager>()->patchNotes.enable)
    {
        if (timeSinceLastUpdate < 0.f && Get<ConfigManager>()->discordSettings.timerType == DiscordSettings::TimerType::TimeUntilPatchNote)
        {
            timeSinceLastUpdate = 15.f;
            const auto lastEffectTimestamp = static_cast<int64>(TimeUtils::UnixTime<std::chrono::seconds>());
            const auto nextEffectTimestamp = lastEffectTimestamp + static_cast<int64>(patchTime);
            Get<DiscordManager>()->SetActivity("Waiting for the next patch", lastEffectTimestamp, nextEffectTimestamp);
        }

        if (Get<ConfigManager>()->patchNotes.countDownWhileOnBases || Utils::GetCShip())
        {
            patchTime -= delta;
            if (patchTime <= 0.0f)
            {
                patchTime = Get<ConfigManager>()->patchNotes.timeBetweenPatchesInMinutes * 60;
                PatchNotes::GeneratePatch();
            }
        }
    }
}

void Change::SetChangeNameAndDescription(const EditableField* field, const FieldData& fieldData, std::string_view itemName, void* oldValue,
    void* newValue, const bool isBuff)
{
    std::string newFieldName = std::regex_replace(field->name, std::regex("([a-z])([A-Z])"), "$1 $2");
    bool lastCharWasSpace = false;
    for (uint i = 0; i < newFieldName.size(); ++i)
    {
        if (lastCharWasSpace || i == 0)
        {
            newFieldName[i] = static_cast<char>(std::toupper(newFieldName[i]));
            lastCharWasSpace = false;
            continue;
        }
        if (newFieldName[i] == ' ')
        {
            lastCharWasSpace = true;
        }
    }

    char symbol = '~';
    if (fieldData.nbb)
    {
        positivity = ChangePositivity::Neither;
    }
    else if (isBuff && !fieldData.inverted)
    {
        positivity = ChangePositivity::Boon;
        symbol = '+';
    }
    else
    {
        positivity = ChangePositivity::Nerf;
        symbol = '-';
    }

    if (field->type == FieldType::Float)
    {
        description = std::format("{} {}: {}   {:.2f}  ->  {:.2f}", symbol, itemName, newFieldName, *(float*)oldValue, *(float*)newValue);
    }
    else if (field->type == FieldType::Int)
    {
        description = std::format("{} {}: {}   {}  ->  {}", symbol, itemName, newFieldName, *(int*)oldValue, *(int*)newValue);
    }
    else if (field->type == FieldType::Bool)
    {
        description = std::format("{} {}: {}   {}  ->  {}", symbol, itemName, newFieldName, *(bool*)oldValue, *(bool*)newValue);
    }
}