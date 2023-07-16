#pragma once

class ChaosOptionText final
{
        bool show = false;

    public:
        void ToggleVisibility();
        void Render() const;
};
