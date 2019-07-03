#pragma once

#include "UtlVector.h"

struct VarEntry final {
    unsigned short type;
    unsigned short needsToInterpolate;
    std::byte pad[8];
};

struct VarMap final {
    UtlVector<VarEntry> entries;
    int interpolatedEntries;
};
