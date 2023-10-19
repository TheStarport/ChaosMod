// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once
#include "Systems/ImguiComponents/ScrollingCredits.hpp"

class RollCredits final : public ActiveEffect
{
        void Begin() override { ScrollingCredits::ToggleVisibility(); }

    public:
        OneShotEffectInfo("Roll Credits", EffectType::Interface);
};
