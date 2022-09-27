#pragma once

#include <cstdint>

struct RetSpoofGadgets {
    RetSpoofGadgets();

    std::uintptr_t jmpEbxInClient = 0;
};

inline RetSpoofGadgets retSpoofGadgets;
