#pragma once

class ImGuiManager;
class ChaosOptionText final
{
        friend ImGuiManager;

        inline static std::vector<std::string> options;

        static void Render()
        {
            if (options.empty())
            {
                return;
            }

            constexpr ImGuiWindowFlags windowFlags =
                ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Always);

            auto location = ImGui::GetIO().DisplaySize;
            location.x = 0 + location.x / 10.0f;
            location.y = 0 + location.y / 6.0f;

            ImGui::SetNextWindowPos(location, ImGuiCond_Always);
            if (!ImGui::Begin("Selection Window", nullptr, windowFlags))
            {
                ImGui::End();
                return;
            }

            for (auto& entry : options)
            {
                ImGui::Text(entry.c_str()); // NOLINT(clang-diagnostic-format-security)
                ImGui::NewLine();
            }

            ImGui::End();
        }

    public:
        ChaosOptionText() = delete;
};