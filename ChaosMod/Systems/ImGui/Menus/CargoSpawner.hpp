#pragma once

#include "ImGuiManager.hpp"
#include "Systems/ConfigManager.hpp"
#include "Systems/PatchNotes/PatchNotes.hpp"
#include "Systems/SystemComponents/TwitchVoting.hpp"
#include "imgui/imgui.h"

#include <magic_enum_utility.hpp>

class ImGuiManager;
class CargoSpawner final
{
        friend ImGuiManager;

        inline static bool show = false;
        inline static std::map<EffectType, std::vector<ActiveEffect*>>* allEffects;

        inline static DWORD* gamePauseIncrementor = PDWORD(0x667D54);
        inline static bool doDecrement = false;

        // Map of type to vector of good id and ids name
        inline static std::unordered_map<Archetype::ClassType, std::vector<std::pair<uint, int>>> goodMap{
            {            Archetype::ClassType::Commodity, {}},
            {                  Archetype::ClassType::Gun, {}},
            {          Archetype::ClassType::MineDropper, {}},
            {Archetype::ClassType::CounterMeasureDropper, {}},
            {             Archetype::ClassType::Thruster, {}},
            {      Archetype::ClassType::ShieldGenerator, {}}
        };

        static void LoadEquipment()
        {
            for (const auto goodList = GoodList_get()->get_list(); const auto* good : *goodList)
            {
                if (good->idsName == 0 || good->price < 1.f)
                {
                    continue;
                }

                if (good->type == GoodInfo::Type::Equipment)
                {
                    const auto equip = Archetype::GetEquipment(good->equipmentId);
                    if (!equip)
                    {
                        continue;
                    }

                    switch (equip->get_class_type())
                    {
                        case Archetype::ClassType::Gun:
                        case Archetype::ClassType::MineDropper:
                        case Archetype::ClassType::CounterMeasureDropper:
                        case Archetype::ClassType::Thruster:
                        case Archetype::ClassType::ShieldGenerator:
                            {
                                goodMap[equip->get_class_type()].emplace_back(good->goodId, good->idsName);
                                break;
                            }
                    }
                }
                else if (good->type == GoodInfo::Type::Commodity)
                {
                    goodMap[Archetype::ClassType::Commodity].emplace_back(good->goodId, good->idsName);
                }
            }
        }

        static void Render()
        {
            if (!show)
            {
                if (doDecrement)
                {
                    *gamePauseIncrementor = 0;
                    doDecrement = false;
                }
                return;
            }

            static bool loaded = false;
            if (!loaded)
            {
                loaded = true;
                LoadEquipment();
            }

            if (!*gamePauseIncrementor)
            {
                *gamePauseIncrementor = 1;
                doDecrement = true;
            }

            ImGui::SetNextWindowSize({ 800.f, 800.f }, ImGuiCond_Always);
            if (!ImGui::Begin("Cargo Spawner", &show, ImGuiWindowFlags_NoResize))
            {
                ImGui::End();
                return;
            }

            static char filter[255];
            ImGui::InputText("Filter", filter, sizeof(filter));

            if (ImGui::BeginTabBar("Cargo Spawning Tabs"))
            {
                for (auto& [equipClass, goods] : goodMap)
                {
                    std::string str;
                    switch (equipClass) // NOLINT
                    {
                        case Archetype::ClassType::Commodity: str = "Cargo"; break;
                        case Archetype::ClassType::CounterMeasureDropper: str = "Countermeasures"; break;
                        case Archetype::ClassType::MineDropper: str = "Mines"; break;
                        case Archetype::ClassType::Gun: str = "Guns/Launchers"; break;
                        case Archetype::ClassType::ShieldGenerator: str = "Shields"; break;
                        case Archetype::ClassType::Thruster: str = "Thrusters"; break;
                        default: ASSERT(false, "Invalid equip class passed into cargo spawner");
                    }

                    if (ImGui::BeginTabItem(str.c_str()))
                    {
                        for (auto& [goodId, ids] : goods)
                        {
                            auto name = GetInfocardName(ids);
                            if (name.empty())
                            {
                                continue;
                            }

                            if (*filter && !strstr(name.c_str(), filter))
                            {
                                continue;
                            }

                            static bool selected = false;
                            ImGui::Selectable(name.c_str(), &selected);
                            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            {
                                if (!Utils::GetShipArch())
                                {
                                    continue;
                                }

                                pub::Player::AddCargo(1, goodId, 1, 1.0f, false);
                                pub::Audio::PlaySoundEffect(1, CreateID("ui_select_add"));
                            }
                        }
                        ImGui::EndTabItem();
                    }
                }

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

    public:
        CargoSpawner() = delete;
};
