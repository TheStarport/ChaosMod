#pragma once

class NpcLoadingException final : std::exception
{
        std::string err;
public:
        explicit NpcLoadingException(const std::string& err) : err(err) {}

        [[nodiscard]] char const* what() const override
        {
            return std::format("Error loading NPC: {}", err).c_str();
        }
};
