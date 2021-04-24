#pragma once

#include <string_view>

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Panel {
public:
    INCONSTRUCTIBLE(Panel)

    constexpr auto getName(unsigned int panel) noexcept
    {
        return std::string_view{ VirtualMethod::call<const char*, 36>(this, panel) };
    }
};
