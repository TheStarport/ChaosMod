// ReSharper disable CppClangTidyClangDiagnosticFormatSecurity
#include "PCH.hpp"

#include "Debug.hpp"

#include <imgui-club/imgui_memory_editor/imgui_memory_editor.h>

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
    editor->GotoAddrAndHighlight(0x8000, len);
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
        for (const auto& [address, length, active] : addressList)
        {
            static bool selected = false;
            std::string str = std::format("{:#010x} ({}) - {}", address, length, active ? "active" : "inactive");
            if (ImGui::Selectable(str.c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick) && ImGui::IsMouseDoubleClicked(0))
            {
                hexWindow.GoTo(address, length);
            }
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

void ActiveAddressList::Refresh() { addressList = MemChange::GetMemChanges(); }

void DebugLog::Render()
{
    addressList.Render();

    if (!show)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
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
    if (ImGui::Button("Open Address List"))
    {
        addressList.show = true;
    }
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);

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

void DebugLog::Log(std::string log)
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
