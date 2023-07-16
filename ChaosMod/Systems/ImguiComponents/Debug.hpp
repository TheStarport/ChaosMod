#pragma once

#include "Utilities/MemChange.hpp"

#include <imgui.h>

// Wrapper around ImGui Memory Editor
class HexWindow final
{
        class MemoryEditor* editor = nullptr;
        std::array<byte, 0x10000> hexBuffer{};
        size_t currentOffset = 0;
        size_t currentMin = 0;
        size_t currentMax = 0;

    public:
        void Render();

        ~HexWindow();
        HexWindow();

        void Refresh();
        void GoTo(DWORD address, size_t len);
};

class ActiveAddressList final
{
        std::vector<MemChange::ActiveChange> addressList{};
        HexWindow hexWindow{};
        bool keepHexWindowUpdated = false;

    public:
        bool show = false;

        void Render();
        void Refresh();

        ActiveAddressList() { Refresh(); }
};

class DebugLog final
{
        ImGuiTextBuffer buf;
        ImGuiTextFilter filter;
        ImVector<int> lineOffsets;

        ActiveAddressList addressList{};

    public:
        DebugLog() { lineOffsets.push_back(0); }

        bool show = true;
        void Render();
        void Log(std::string);
};
