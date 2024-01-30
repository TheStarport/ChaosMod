#include "PCH.hpp"

#include "PatchNotes.hpp"

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <neargye/semver.hpp>

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
                ResetPatches(false);
                availablePatches.clear();
                patchNotes.clear();

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

void PatchNotes::SavePatches() const
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

void PatchNotes::RevertPatch(std::shared_ptr<Patch> patch)
{
    patchNotes.pop_back();

    for (const auto& change : patch->changes)
    {
        change->Revert();
    }

    appliedPatches.pop();
}

std::string_view PatchNotes::GetPatchNotes() const
{
    static std::string str;
    str.clear();

    for (const auto& patch : patchNotes)
    {
        str += patch;
    }

    return str;
}

void PatchNotes::ResetPatches(const bool reapply)
{
    while (!appliedPatches.empty())
    {
        const auto patch = appliedPatches.top();
        RevertPatch(patch);
    }

    if (reapply)
    {
        for (const auto& patch : availablePatches)
        {
            ApplyPatch(patch);
        }
    }
}

void PatchNotes::ApplyPatch(std::shared_ptr<Patch> patch)
{
    appliedPatches.push(patch);

    std::string note;
    for (const auto& change : patch->changes)
    {
        note += change->description + "\n";
        change->Apply();
    }

    patchNotes.emplace_back(note);
}

void PatchNotes::GeneratePatch()
{
    auto changeCount = Random::i()->Uniform(2u, 10u);

    semver::version lastVersion{ "1.0.0" };
    if (!availablePatches.empty())
    {
        const auto& lastPatch = availablePatches.back();
        lastVersion = semver::from_string(lastPatch->version);
    }

    const auto versionIncrement = Random::i()->Uniform(1u, 30u);
    if (versionIncrement < 25 && lastVersion.patch != 255)
    {
        lastVersion.patch++;
    }
    else if (versionIncrement < 30 && lastVersion.minor != 255)
    {
        lastVersion.minor++;
        lastVersion.patch = 0;
    }
    else
    {
        lastVersion.patch = 0;
        lastVersion.minor = 0;
        lastVersion.major++;
    }

    auto patch = std::make_shared<Patch>();
    patch->version = lastVersion.to_string();
    patch->date = TimeUtils::UnixTime<std::chrono::seconds>();

    for (auto i = 0u; i < changeCount; ++i)
    {
        auto type = Random::i()->Uniform(1u, magic_enum::enum_count<ChangeType>());
        const auto change = GetChangePtr(static_cast<ChangeType>(type));

        change->Generate();
        patch->changes.emplace_back(change);
    }

    availablePatches.emplace_back(patch);
    ApplyPatch(patch);
    SavePatches();
}

std::shared_ptr<Change> PatchNotes::GetChangePtr(const ChangeType type)
{
    std::shared_ptr<Change> change;

    // clang-format off
    switch (type)
    {
        case ChangeType::Gun: 
            change = std::make_shared<GunChange>();
            break;
        default:   // NOLINT(clang-diagnostic-covered-switch-default)
            ASSERT(false, "Invalid type provided in change log.");
    }
    // clang-format on

    return change;
}

PatchNotes::PatchNotes() { LoadPatches(); }
