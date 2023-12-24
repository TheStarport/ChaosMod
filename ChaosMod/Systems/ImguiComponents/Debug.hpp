#pragma once

#include "EffectToggler.hpp"
#include "Effects/MemoryEffect.hpp"

#include <imgui/imgui.h>

class EffectSelectorWindow final
{
        std::map<EffectType, std::vector<ActiveEffect*>>* allEffects;

    public:
        explicit EffectSelectorWindow(std::map<EffectType, std::vector<ActiveEffect*>>* effects) : allEffects(effects) {}

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
        std::map<EffectType, std::vector<ActiveEffect*>> allEffects;

        ImGuiTextBuffer buf;
        ImGuiTextFilter filter;
        ImVector<int> lineOffsets;

        ActiveAddressList addressList{};
        Configurator configurator{};
        EffectSelectorWindow effectSelector{ &allEffects };
        EffectToggler effectToggler{ &allEffects };

    public:
        DebugMenu();

        bool show =
#ifdef _DEBUG
            true;
#else
            false;
#endif

        void Render();
        void Log(std::string);
};
