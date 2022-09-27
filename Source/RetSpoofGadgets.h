#pragma once

#include <cstdint>

struct RetSpoofGadgets {
    RetSpoofGadgets();

    std::uintptr_t jmpEbxInClient = 0;
};

#ifdef _WIN32
inline RetSpoofGadgets retSpoofGadgets;
#endif
