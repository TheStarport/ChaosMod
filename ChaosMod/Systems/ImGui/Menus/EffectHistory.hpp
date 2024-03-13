#pragma once
#include "Systems/ChaosTimer.hpp"
#include "imgui/imgui.h"

class ImGuiManager;
class EffectHistory final
{
        friend ImGuiManager;

        inline static bool show = false;
        struct PastEffectData
        {
                std::string_view name;
                std::string_view description;
                std::string timestamp;
        };

        inline static std::vector<PastEffectData> pastEffects;

        static void Render()
        {
            if (!show)
            {
                return;
            }

            ImGui::SetNextWindowSize({ 800.f, 800.f }, ImGuiCond_Always);
            if (!ImGui::Begin("Effect History", &show, ImGuiWindowFlags_NoResize))
            {
                ImGui::End();
                return;
            }

            ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, ImVec2(0.45f, 0.5f));
            ImGui::SeparatorText("Currently Persisting Effects");

            auto& persistentEffects = ChaosTimer::GetActivePersistentEffects();

            bool persisting = false;
            if (ImGui::BeginTable("PersistingEffectTable", 3, ImGuiTableFlags_SizingStretchProp))
            {
                for (auto& effect : persistentEffects)
                {
                    if (!effect->Persisting())
                    {
                        continue;
                    }

                    auto& info = effect->GetEffectInfo();
                    ImGui::TableNextColumn();
                    ImGui::Text(info.effectName.c_str());

                    ImGui::TableNextColumn();
                    if (ImGui::Button("Cancel##Persist"))
                    {
                        dynamic_cast<ActiveEffect*>(effect)->End();
                    }

                    ImGui::TableNextColumn();
                    ImGui::TextWrapped(info.description.c_str());

                    persisting = true;
                }

                ImGui::EndTable();
            }

            if (!persisting)
            {
                constexpr std::string_view text = "No effects are currently persisting.";
                const auto windowWidth = ImGui::GetWindowSize().x;
                const auto textWidth = ImGui::CalcTextSize(text.data()).x;

                ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
                ImGui::Text(text.data());
            }

            ImGui::SeparatorText("Currently Active Effects");

            auto& activeEffects = ChaosTimer::GetActiveEffects();
            if (activeEffects.empty())
            {
                constexpr std::string_view text = "No effects are currently active.";
                const auto windowWidth = ImGui::GetWindowSize().x;
                const auto textWidth = ImGui::CalcTextSize(text.data()).x;

                ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
                ImGui::Text(text.data());
            }
            else if (ImGui::BeginTable("ActiveEffectsTable", 3, ImGuiTableFlags_SizingStretchProp))
            {
                /*ImGui::TableSetupColumn("ActiveEffectsColumn0");
                ImGui::TableSetupColumn("ActiveEffectsColumn1");
                ImGui::TableSetupColumn("ActiveEffectsColumn2", ImGuiTableColumnFlags_WidthStretch);*/
                for (const auto& effect : activeEffects | std::views::keys)
                {
                    auto& info = effect->GetEffectInfo();

                    ImGui::TableNextColumn();
                    ImGui::Text(info.effectName.c_str());

                    ImGui::TableNextColumn();
                    if (ImGui::Button("Cancel##Active"))
                    {
                        ChaosTimer::EndEffectPrematurely(effect);
                    }

                    ImGui::TableNextColumn();
                    ImGui::TextWrapped(info.description.c_str());
                }

                ImGui::EndTable();
            }

            ImGui::SeparatorText("Past Effects Effects");
            ImGui::PopStyleVar();

            if (ImGui::BeginTable("PastEffectsTable", 3, ImGuiTableFlags_SizingStretchProp))
            {
                for (auto data = pastEffects.rbegin(); data != pastEffects.rend(); ++data)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text(data->timestamp.data());

                    ImGui::TableNextColumn();
                    ImGui::Text(data->name.data());

                    ImGui::TableNextColumn();
                    ImGui::TextWrapped(data->description.data());
                }

                ImGui::EndTable();
            }

            ImGui::End();
        }
};
