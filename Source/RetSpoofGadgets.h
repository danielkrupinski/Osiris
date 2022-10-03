#pragma once

#include <cstdint>

#include "Platform/RetSpoofInvoker.h"

struct RetSpoofGadgets {
    RetSpoofGadgets();

    RetSpoofInvoker jmpEbxInClient;
    RetSpoofInvoker engine;
};

inline RetSpoofGadgets retSpoofGadgets;
