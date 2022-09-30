#pragma once

#include <cstdint>

#include "Platform/RetSpoofInvoker.h"

struct RetSpoofGadgets {
    RetSpoofGadgets();

    RetSpoofInvoker jmpEbxInClient;
};

inline RetSpoofGadgets retSpoofGadgets;
