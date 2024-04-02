#pragma once

#include "CoreComponents/Patches/Patch.hpp"

class PatchNotes
{
        inline static float patchTime;
        struct StringPatch
        {
                std::string version;
                std::string date;
                std::vector<std::pair<std::string, Change::ChangePositivity>> changes;
                std::string releaseName;
        };

        inline static std::vector<StringPatch> patchNotes;
        inline static std::stack<std::shared_ptr<Patch>> appliedPatches;
        inline static std::vector<std::shared_ptr<Patch>> availablePatches;
        static void SavePatches();
        static void RevertPatch(const std::shared_ptr<Patch> &patch);
        static void ApplyPatch(const std::shared_ptr<Patch> &patch, bool showPatchNotes = true);
        static std::shared_ptr<Change> GetChangePtr(ChangeType type);
        inline static Random randomEngine;

    public:
        enum class PatchVersion
        {
            Undetermined,
            Patch,
            Minor,
            Major
        };

        static void Update(float delta);
        static void LoadPatches();
        static std::vector<StringPatch> &GetPatchNotes();
        static void ResetPatches(bool reapply, bool clean);
        static void GeneratePatch(PatchVersion version = PatchVersion::Undetermined);
        static void DoubleDown();
        static void RevertLastPatch();
        static ChangeType GetRandomChangeType();
        static void Reseed(std::string_view seed);
        static void Reseed(std::istream &seed);
};
