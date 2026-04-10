#pragma once

#include "Changes.hpp"

struct Patch
{
        std::string releaseName;
        std::string version;
        unsigned64 date;
        //std::vector<std::shared_ptr<Change>> changes;
};
