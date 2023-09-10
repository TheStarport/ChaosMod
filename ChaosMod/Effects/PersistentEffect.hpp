#pragma once
#include "ActiveEffect.hpp"

class PersistentEffect : public ActiveEffect
{
        bool persisting = false;

    protected:
        void Begin() override { persisting = true; }
        void End() override { persisting = false; }

    public:
        [[nodiscard]]
        bool Persisting() const
        {
            return persisting;
        }
};
