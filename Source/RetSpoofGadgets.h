#pragma once

#include <cstdint>

#include "Platform/RetSpoofInvoker.h"

struct RetSpoofGadgets {
    RetSpoofGadgets();

    RetSpoofInvoker client;
    RetSpoofInvoker engine;
};

inline RetSpoofGadgets retSpoofGadgets;
