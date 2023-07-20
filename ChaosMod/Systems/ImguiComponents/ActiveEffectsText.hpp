#pragma once

class ActiveEffectsText final
{
        static bool boxOfChocolates;
        bool show = true;

        void WriteEffect(const std::string& name, const bool isTimed, const float time, const float modifier) const;

    public:
        static void BoxOfChocolates();
        void ToggleVisibility();
        void Render() const;
};
