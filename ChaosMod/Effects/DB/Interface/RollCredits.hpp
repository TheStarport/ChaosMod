// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once
#include "Systems/ImguiComponents/ScrollingCredits.hpp"

class RollCredits final : public ActiveEffect
{
        void Begin() override { ScrollingCredits::ToggleVisibility(); }

    public:
        DefEffectInfo("Roll Credits", 0.0f, EffectType::Interface);
};
