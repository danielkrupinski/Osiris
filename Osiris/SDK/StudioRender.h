#pragma once

#include <cstddef>

#include "Material.h"
#include "../Memory.h"

class StudioRender {
    std::byte pad_0[0x250];
    Material* materialOverride;
    std::byte pad_1[0xC];
    int overrideType;
public:
    bool isInGlow() noexcept
    {
        if (!materialOverride)
            return overrideType != 0; // other than OVERRIDE_NORMAL
        return std::strstr(materialOverride->getName(), "dev/glow") != nullptr;
    }
};
