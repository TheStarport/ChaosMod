// ReSharper disable CppClangTidyClangDiagnosticUnusedPrivateField
#pragma once
#include "Systems/HudInterface.hpp"

class FlippedUi final : public ActiveEffect
{
        void Begin() override { HudInterface::i()->ToggleFlipping(); }
        void End() override { HudInterface::i()->ToggleFlipping(); }

    public:
        DefaultEffectInfo("Flipped Interface", EffectType::Interface);
};
