#pragma once

#include "UtlVector.h"

struct VarEntry {
    unsigned short type;
    unsigned short needsToInterpolate;
    std::byte pad[8];
};

struct VarMap {
    UtlVector<VarEntry> entries;
    int interpolatedEntries;
};
