#pragma once

#include <string_view>
#include "VirtualMethod.h"

class Panel {
public:
    constexpr auto getName(unsigned int panel) noexcept
    {
        return std::string_view{ VirtualMethod::call<const char*, 36>(this, panel) };
    }
};
