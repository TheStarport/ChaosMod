#pragma once

#include "Patch.hpp"

class PatchNotes : Singleton<PatchNotes>
{
        struct StringPatch
        {
                std::string version;
                std::string date;
                std::vector<std::string> changes;
                std::string releaseName;
        };

        inline static std::vector<StringPatch> patchNotes;
        inline static std::stack<std::shared_ptr<Patch>> appliedPatches;
        inline static std::vector<std::shared_ptr<Patch>> availablePatches;
        static void SavePatches();
        static void RevertPatch(const std::shared_ptr<Patch> &patch);
        static void ApplyPatch(const std::shared_ptr<Patch> &patch, bool showPatchNotes = true);
        static std::shared_ptr<Change> GetChangePtr(ChangeType type);

    public:
        static void LoadPatches();
        static std::vector<StringPatch> &GetPatchNotes();
        static void ResetPatches(bool reapply, bool clean);
        static void GeneratePatch();
        static ChangeType GetRandomChangeType();
};
