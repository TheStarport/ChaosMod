#pragma once
#include "Effects/ActiveEffect.hpp"
#include "imgui.h"

#include <magic_enum_utility.hpp>

class ImGuiManager;
class EffectSelector final
{
        friend ImGuiManager;

        inline static std::map<EffectType, std::vector<ActiveEffect*>>* allEffects;
        inline static bool show = false;
        static void Render()
        {
            if (!show)
            {
                return;
            }

            ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);
            ImGui::Begin("Effect Selector", &show);

            magic_enum::enum_for_each<EffectType>(
                [](auto val)
                {
                    constexpr EffectType category = val;
                    const auto categoryName = std::string(magic_enum::enum_name(category));

                    const auto effects = allEffects->find(category);
                    if (effects == allEffects->end())
                    {
                        return;
                    }

                    if (ImGui::TreeNode(categoryName.c_str()))
                    {
                        for (ActiveEffect* effect : effects->second)
                        {
                            auto& info = effect->GetEffectInfo();
                            static bool selected = false;
                            if (ImGui::Selectable(info.effectName.c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick) && ImGui::IsMouseDoubleClicked(0))
                            {
                                Get<ChaosTimer>()->TriggerChaos(effect);
                            }
                            selected = false;
                        }

                        ImGui::TreePop();
                    }

                    ImGui::Separator();
                });

            ImGui::End();
        }

    public:
        EffectSelector() = delete;
};
