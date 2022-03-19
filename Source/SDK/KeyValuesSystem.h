#pragma once

#include "VirtualMethod.h"

class KeyValuesSystem {
public:
    VIRTUAL_METHOD(const char*, getStringForSymbol, 4, (int symbol), (this, symbol))
};
