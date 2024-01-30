#pragma once

#include "Changes.hpp"

struct Patch
{
        std::string version;
        uint64 date;
        std::vector<std::shared_ptr<Change>> changes;
};
