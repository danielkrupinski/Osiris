#pragma once

#include <cstddef>

#include "Material.h"

class StudioRender {
    std::byte pad[0x250];
    Material* materialOverride;
public:
    bool isInGlow() noexcept
    {
        if (!materialOverride)
            return false;

        return std::strstr(materialOverride->getName(), "dev/glow") != nullptr;
    }
};
