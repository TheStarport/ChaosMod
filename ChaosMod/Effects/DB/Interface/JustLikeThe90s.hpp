// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once
#include "Systems/UiManager.hpp"

class JustLikeThe90s final : public ActiveEffect
{
        std::vector<HCURSOR> cursors;
        void Begin() override { UiManager::i()->OverrideCursor(cursors[Random::i()->Uniform(0u, cursors.size() - 1)]); }
        void End() override { UiManager::i()->OverrideCursor(std::nullopt); }

    public:
        JustLikeThe90s()
        {
            char szCurDir[MAX_PATH];
            GetCurrentDirectoryA(sizeof(szCurDir), szCurDir);

            for (const auto dir = std::string(szCurDir); const auto& entry : std::filesystem::recursive_directory_iterator(dir + "/../DATA/CHAOS/CURSORS"))
            {
                if (!entry.is_regular_file() || entry.file_size() > UINT_MAX ||
                    (!(entry.path().generic_string().ends_with(".cur")) && !(entry.path().generic_string().ends_with(".ani"))))
                {
                    continue;
                }

                const HCURSOR hCur = LoadCursorFromFileA(entry.path().generic_string().c_str());
                if (!hCur)
                {
                    continue;
                }

                cursors.emplace_back(hCur);
            }
        }

        DefEffectInfo("Just Like The 90s", 1.0f, EffectType::Interface);
};
