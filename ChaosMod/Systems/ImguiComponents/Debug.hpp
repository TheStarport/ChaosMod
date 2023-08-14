#pragma once

#include "Effects/MemoryEffect.hpp"

#include <imgui.h>

class EffectSelectorWindow final
{
        std::map<EffectType, std::vector<ActiveEffect*>> allEffects;

    public:
        void Refresh();
        EffectSelectorWindow();

        bool show = false;
        void Render();
};

class ActiveAddressList final
{
        std::vector<MemoryEffect::MemoryAddress> addressList;
        bool keepHexWindowUpdated = false;

    public:
        bool show = false;

        void Render();
        void Refresh();

        ActiveAddressList() { Refresh(); }
};

class Configurator final
{
    public:
        bool show = false;
        void Render();
};

class DebugMenu final
{
        ImGuiTextBuffer buf;
        ImGuiTextFilter filter;
        ImVector<int> lineOffsets;

        ActiveAddressList addressList{};
        Configurator configurator{};
        EffectSelectorWindow effectSelector{};

    public:
        DebugMenu() { lineOffsets.push_back(0); }

        bool show = true;
        void Render();
        void Log(std::string);
};
