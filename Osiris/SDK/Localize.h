#pragma once

#include "VirtualMethod.h"

class Localize {
public:
    VIRTUAL_METHOD(const wchar_t*, find, 12, (const char* tokenName), (this, tokenName))
};
