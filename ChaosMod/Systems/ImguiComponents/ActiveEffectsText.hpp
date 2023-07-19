#pragma once

class ActiveEffectsText final
{
        bool show = true;

    public:
        void ToggleVisibility();
        void Render() const;
};
