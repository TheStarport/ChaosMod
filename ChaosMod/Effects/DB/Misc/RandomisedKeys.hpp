#pragma once
#include "Effects/ActiveEffect.hpp"
#include "Systems/KeyManager.hpp"

class RandomisedKeys final : public ActiveEffect
{
        void Begin() override { KeyManager::i()->ToggleRandomisedKeys(); }
        void End() override { KeyManager::i()->ToggleRandomisedKeys(); }

    public:
        DefEffectInfo("Randomised Keys", 1.0f, EffectType::Misc);
};
