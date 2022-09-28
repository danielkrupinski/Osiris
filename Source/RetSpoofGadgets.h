#pragma once

#include <cstdint>

#include "RetSpoofInvoker.h"

struct RetSpoofGadgets {
    RetSpoofGadgets();

    RetSpoofInvoker jmpEbxInClient;
};

#ifdef _WIN32
inline RetSpoofGadgets retSpoofGadgets;
#endif
