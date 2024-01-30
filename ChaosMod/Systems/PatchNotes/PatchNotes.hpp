#pragma once

#include "Patch.hpp"

class PatchNotes : Singleton<PatchNotes>
{
        std::vector<std::string> patchNotes;
        std::stack<std::shared_ptr<Patch>> appliedPatches;
        std::vector<std::shared_ptr<Patch>> availablePatches;
        void SavePatches() const;
        void RevertPatch(std::shared_ptr<Patch> patch);
        void ApplyPatch(std::shared_ptr<Patch> patch);
        static std::shared_ptr<Change> GetChangePtr(ChangeType type);

    public:
        void LoadPatches();
        std::string_view GetPatchNotes() const;
        void ResetPatches(bool reapply);
        void GeneratePatch();
        PatchNotes();
};
