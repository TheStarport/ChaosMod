#include "PCH.hpp"

#include "EffectToggler.hpp"

#include "Effects/ActiveEffect.hpp"
#include "Systems/ConfigManager.hpp"

#include <imgui/imgui.h>
#include <magic_enum_utility.hpp>

void EffectToggler::RenderEffectCategory()
{
    bool t = true;
    ImGui::Checkbox("Test", &t);
    ImGui::SameLine();
    ImGui::SeparatorText("TEST");
}

void EffectToggler::Show() { show = true; }

void EffectToggler::Render()
{
    if (!show)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Effect Toggle", &show);

    auto& configEffects = ConfigManager::i()->toggledEffects;

    magic_enum::enum_for_each<EffectType>(
        [this, &configEffects](auto val)
        {
            constexpr EffectType category = val;
            const auto categoryName = std::string(magic_enum::enum_name(category));

            const auto categoryEffects = effects->find(category);
            if (categoryEffects == effects->end())
            {
                return;
            }

            bool configEffectsNeedPopulating = false;
            if (!configEffects.contains(categoryName))
            {
                configEffects[categoryName] = {};
                configEffectsNeedPopulating = true;
            }

            auto& configCategoryEffects = configEffects[categoryName];

            ImGui::PushID(categoryName.c_str());
            bool all = std::all_of(configCategoryEffects.begin(),
                                   configCategoryEffects.end(),
                                   [](std::pair<const std::string, bool>& individualEffect) { return individualEffect.second; });

            if (ImGui::Checkbox("All?##ID", &all))
            {
                for (auto& i : configCategoryEffects)
                {
                    i.second = all;
                }

                ConfigManager::i()->Save();
            }

            ImGui::SameLine();
            ImGui::SeparatorText(categoryName.c_str());

            int i = -1;
            for (ActiveEffect* effect : categoryEffects->second)
            {
                auto& info = effect->GetEffectInfo();

                if (configEffectsNeedPopulating)
                {
                    configEffects[categoryName][info.effectName] = true;
                }

                auto& isEnabled = configEffects[categoryName][info.effectName];
                if (i++ >= 0 && i < 3)
                {
                    ImGui::SameLine();
                }
                else
                {
                    i = 0;
                    ImGui::Dummy(ImVec2(50.0f, 50.0f));
                    ImGui::SameLine();
                }

                if (ImGui::Checkbox(info.effectName.c_str(), &isEnabled))
                {
                    ConfigManager::i()->Save();
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip(info.description.c_str()); // NOLINT(clang-diagnostic-format-security)
                }
            }

            if (configEffectsNeedPopulating)
            {
                ConfigManager::i()->Save();
            }

            ImGui::PopID();
        });

    ImGui::End();
}
