#pragma once

#include <string_view>
#include "Utils.h"

class Panel {
public:
    constexpr auto getName(unsigned int panel) noexcept
    {
        return std::string_view{ callVirtualMethod<const char*, unsigned int>(this, 36, panel) };
    }
};
