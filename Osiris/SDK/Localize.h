#pragma once

#include "VirtualMethod.h"

class Localize {
public:
    VIRTUAL_METHOD(const wchar_t*, find, 12, (const char* tokenName), (this, tokenName))
    VIRTUAL_METHOD(const char*, findAsUTF8, 47, (const char* tokenName), (this, tokenName))
};
