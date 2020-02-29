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
    bool isForcedMaterialOverride() noexcept
    {
        if (!materialOverride)
            return overrideType == 2 || overrideType == 4; // see CStudioRenderContext::IsForcedMaterialOverride
        return std::strstr(materialOverride->getName(), "dev/glow") != nullptr;
    }
};
