#include "PCH.hpp"

#include "ActiveEffectsText.hpp"

#include "Systems/ChaosTimer.hpp"
#include "Systems/ConfigManager.hpp"
#include "Systems/UiManager.hpp"

#include <imgui.h>

bool ActiveEffectsText::boxOfChocolates = false;

void ActiveEffectsText::BoxOfChocolates() { boxOfChocolates = !boxOfChocolates; }
void ActiveEffectsText::ToggleVisibility() { show = !show; }

void ActiveEffectsText::WriteEffect(const std::string& name, const bool isTimed, const float time, const float modifier) const
{
    ImGui::Text(name.c_str()); // NOLINT

    if (isTimed)
    {
        const float fraction = time / (modifier * ConfigManager::i()->defaultEffectDuration);

        ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 200);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ConfigManager::i()->progressBarColor);
        ImGui::ProgressBar(fraction, ImVec2(200, 0), "");
        ImGui::PopStyleColor();
    }

    ImGui::NewLine();
}

void ActiveEffectsText::Render() const
{
    if (!show)
    {
        return;
    }

    constexpr ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

    auto location = ImGui::GetIO().DisplaySize;
    location.x = location.x - location.x / 4.0f;

    ImGui::SetNextWindowSize(ImVec2(500, location.y - (location.y * 10 / 100)), ImGuiCond_Always);

    location.y = 200;
    ImGui::SetNextWindowPos(location, ImGuiCond_Always);
    if (!ImGui::Begin("Active Effects Window", nullptr, windowFlags))
    {
        ImGui::End();
        return;
    }

    if (boxOfChocolates)
    {
        auto effects = ChaosTimer::GetActiveEffects();
        if (const auto effect = std::ranges::find_if(effects, [](auto& ef) { return ef.first->GetEffectInfo().effectName == "Box Of Chocolates"; });
            effect == effects.end())
        {
            Log("Box of chocolates was active, but no effect was found.");
            boxOfChocolates = false;
        }
        else
        {
            const auto& info = effect->first->GetEffectInfo();
            WriteEffect(info.effectName, info.isTimed, effect->second, info.timingModifier);
            ImGui::End();
            return;
        }
    }

    for (const auto& effects = ChaosTimer::GetActiveEffects(); auto& effect : effects)
    {
        const auto& info = effect.first->GetEffectInfo();
        WriteEffect(info.effectName, info.isTimed, effect.second, info.timingModifier);
    }

    ImGui::End();
}
