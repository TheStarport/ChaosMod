// ReSharper disable CppClangTidyClangDiagnosticFormatSecurity
#include "PCH.hpp"

#include "Debug.hpp"

#include "Systems/ChaosTimer.hpp"
#include "Systems/ConfigManager.hpp"

#include <imgui-club/imgui_memory_editor/imgui_memory_editor.h>
#include <magic_enum.hpp>

void EffectSelectorWindow::Refresh()
{
    for (const auto effects = ActiveEffect::GetAllEffects(); const auto& effect : effects)
    {
        auto& info = effect->GetEffectInfo();
        if (!allEffects.contains(info.category))
        {
            allEffects[info.category] = {};
        }

        allEffects[info.category].emplace_back(effect);
    }
}

EffectSelectorWindow::EffectSelectorWindow() { Refresh(); }

void EffectSelectorWindow::Render()
{
    if (!show)
    {
        return;
    }

    if (allEffects.empty())
    {
        Refresh();
    }

    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Effect Selector", &show);

    magic_enum::enum_for_each<EffectType>(
        [this](auto val)
        {
            constexpr EffectType category = val;
            const auto categoryName = std::string(magic_enum::enum_name(category));

            const auto effects = allEffects.find(category);
            if (effects == allEffects.end())
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
                        ChaosTimer::i()->TriggerSpecificEffect(effect);
                    }
                    selected = false;
                }

                ImGui::TreePop();
            }

            ImGui::Separator();
        });

    ImGui::End();
}

void HexWindow::Render() { editor->DrawWindow("Hex View", hexBuffer.data(), hexBuffer.size(), currentOffset); }
HexWindow::~HexWindow() { delete editor; }
HexWindow::HexWindow() { editor = new MemoryEditor(); }
void HexWindow::Refresh()
{
    DWORD range = currentMax - currentMin;
    if (range != 0x10000)
    {
        range += std::abs((int)range - 0x10000);
    }

    Utils::Memory::ReadProcMem(PBYTE(currentMin), PBYTE(hexBuffer.data()), range);
}

void HexWindow::GoTo(const DWORD address, const size_t len)
{
    currentMin = address < 0x8000 ? 0 : address - 0x8000;
    currentMax = address + 0x8000 - 1;
    currentOffset = address;

    Refresh();
    editor->GotoAddrAndHighlight(0x8000, 0x8000 + len);
}

void ActiveAddressList::Render()
{
    if (!show)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Active Address List", &show))
    {
        ImGui::End();
    }

    if (ImGui::Button("Refresh"))
    {
        Refresh();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Keep Hex Window Updated", &keepHexWindowUpdated);

    ImGui::Separator();
    if (ImGui::BeginChild("address-list", ImVec2(0, 0), false))
    {
        for (const auto& [module, offset, length, originalData] : addressList)
        {
            static bool selected = false;
            std::string str = std::format("{:#010x} ({})", module + offset, length);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, 0x660500FF);
            if (ImGui::Selectable(str.c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick) && ImGui::IsMouseDoubleClicked(0))
            {
                hexWindow.GoTo(module + offset, length);
            }
            ImGui::PopStyleColor();
            selected = false;
        }
    }

    ImGui::EndChild();
    ImGui::End();

    if (keepHexWindowUpdated)
    {
        hexWindow.Refresh();
    }

    hexWindow.Render();
}

void ActiveAddressList::Refresh() { addressList = MemoryEffect::GetMemoryEffects(); }

void Configurator::Render()
{
    if (!show)
    {
        return;
    }

    if (!ImGui::Begin("Configurator", &show, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    auto* config = ConfigManager::i();
    if (ImGui::Button("Save Changes"))
    {
        config->Save();
    }

    ImGui::Separator();

    // Effect Settings
    ImGui::SliderFloat("Time Between Chaos", &config->timeBetweenChaos, 5.0f, 120.0f);
    ImGui::SliderFloat("Default Effect Duration", &config->defaultEffectDuration, 20.0f, 300.0f);
    ImGui::SliderInt("Total Allowed Concurrent Effects", reinterpret_cast<PINT>(&config->totalAllowedConcurrentEffects), 1, 12);
    ImGui::Separator();

    // Meta Settings
    ImGui::Text("Meta Settings");
    ImGui::Checkbox("Allow Meta Effects", &config->allowMetaEffects);
    ImGui::Separator();

    // Twitch Settings
    ImGui::Text("Twitch Settings");

    ImGui::Checkbox("Enable Twitch Voting", &config->enableTwitchVoting);
    ImGui::SameLine();
    ImGui::SliderFloat("Twitch Voting Weight", &config->baseTwitchVoteWeight, 0.1f, 1.0f);
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "When using Twitch votes, the item with the most votes will have this percentage chance of being selected.\n This value will determine "
            "the percentage chance of the Twitch value being selected. If set to 1, Twitch chat's option is guaranteed.\n This option is likely to "
            "be more preferable if you have fewer engaged viewers as to still allow for a degree of randomness.");
    }
    ImGui::Separator();

    // Style Settings
    ImGui::Text("Style Settings");

    static DWORD startingProgressBarColor = ConfigManager::i()->progressBarColor;
    static auto startingProgressBarColorVec = ImGui::ColorConvertU32ToFloat4(startingProgressBarColor);
    static float progressColor[3] = { startingProgressBarColorVec.x, startingProgressBarColorVec.y, startingProgressBarColorVec.z };
    ImGui::ColorEdit3("Chaos Progress Color", progressColor, ImGuiColorEditFlags_PickerHueBar);
    ConfigManager::i()->progressBarColor = ImGui::ColorConvertFloat4ToU32(ImVec4(progressColor[0], progressColor[1], progressColor[2], 0xFF));

    static DWORD startingProgressBarTextColor = ConfigManager::i()->progressBarTextColor;
    static auto startingProgressBarTextColorVec = ImGui::ColorConvertU32ToFloat4(startingProgressBarTextColor);
    static float progressTextColor[3] = { startingProgressBarTextColorVec.x, startingProgressBarTextColorVec.y, startingProgressBarTextColorVec.z };
    ImGui::ColorEdit3("Chaos Progress Text Color", progressTextColor, ImGuiColorEditFlags_PickerHueBar);
    ConfigManager::i()->progressBarTextColor = ImGui::ColorConvertFloat4ToU32(ImVec4(progressTextColor[0], progressTextColor[1], progressTextColor[2], 0xFF));

    ImGui::Checkbox("Show Time Remaining On Effects", &ConfigManager::i()->showTimeRemainingOnEffects);

    ImGui::End();
}

void DebugMenu::Render()
{
    addressList.Render();
    configurator.Render();
    effectSelector.Render();

    if (!show)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Debug Console", &show))
    {
        ImGui::End();
        return;
    }

    ImGui::SameLine();
    const bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    const bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);

    if (ImGui::Button("Open Address List"))
    {
        addressList.show = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Open Configurator"))
    {
        configurator.show = true;
    }

    if (ImGui::Button("Open Debug Event Selector"))
    {
        effectSelector.show = true;
    }

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (clear)
        {
            buf.clear();
            lineOffsets.clear();
            lineOffsets.push_back(0);
        }
        if (copy)
        {
            ImGui::LogToClipboard();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* b = buf.begin();
        const char* bend = buf.end();
        if (filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have random access to the result of our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int lineNo = 0; lineNo < lineOffsets.Size; lineNo++)
            {
                const char* lineStart = b + lineOffsets[lineNo];
                if (const char* lineEnd = (lineNo + 1 < lineOffsets.Size) ? (b + lineOffsets[lineNo + 1] - 1) : bend; filter.PassFilter(lineStart, lineEnd))
                {
                    ImGui::TextUnformatted(lineStart, lineEnd);
                }
            }
        }
        else
        {
            ImGuiListClipper clipper;
            clipper.Begin(lineOffsets.Size);
            while (clipper.Step())
            {
                for (int lineNo = clipper.DisplayStart; lineNo < clipper.DisplayEnd; lineNo++)
                {
                    const char* lineStart = b + lineOffsets[lineNo];
                    const char* lineEnd = (lineNo + 1 < lineOffsets.Size) ? (b + lineOffsets[lineNo + 1] - 1) : bend;
                    ImGui::TextUnformatted(lineStart, lineEnd);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void DebugMenu::Log(std::string log)
{
    log += "\n";
    int oldSize = buf.size();
    buf.append(log.c_str(), log.c_str() + log.size());
    for (const int newSize = buf.size(); oldSize < newSize; oldSize++)
    {
        if (buf[oldSize] == '\n')
        {
            lineOffsets.push_back(oldSize + 1);
        }
    }
}
