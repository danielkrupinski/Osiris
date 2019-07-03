#pragma once

#include "Utils.h"

class Localize {
public:
    constexpr auto find(const char* tokenName) noexcept
    {
        return callVirtualMethod<wchar_t*, const char*>(this, 11, tokenName);
    }
};