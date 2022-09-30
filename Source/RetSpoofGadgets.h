#pragma once

#include <cstdint>

#include "Platform/Windows/RetSpoofInvoker.h"

struct RetSpoofGadgets {
    RetSpoofGadgets();

    windows_platform::RetSpoofInvoker jmpEbxInClient;
};

#ifdef _WIN32
inline RetSpoofGadgets retSpoofGadgets;
#endif
