#pragma once

class ChaosOptionText final
{
        std::vector<std::string> options;

    public:
        void SetOptions(const std::vector<std::string>& options);
        void Render() const;
};
