#pragma once

#include <string_view>
#include "Utils.h"

class Panel {
public:
    constexpr auto getName(unsigned int panel) noexcept
    {
        return std::string_view{ callVirtualFunction<const char*, void*, unsigned int>(this, 36)(this, panel) };
    }
};
