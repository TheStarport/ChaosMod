#pragma once
#include "Effects/PersistentEffect.hpp"
#include "Systems/ReshadeManager.hpp"

class Nightvision final : public ActiveEffect
{
        void Begin() override { ReshadeManager::i()->ToggleTechnique("Nightvision", true); }

        void End() override { ReshadeManager::i()->ToggleTechnique("Nightvision", false); }

    public:
        DefaultEffectInfo("Nightvision", EffectType::Visual);
};
