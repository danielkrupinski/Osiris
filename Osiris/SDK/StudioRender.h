#pragma once

#include <cstddef>
#include <string_view>

#include "Material.h"
#include "VirtualMethod.h"

enum class OverrideType {
    Normal = 0,
    BuildShadows,
    DepthWrite,
    CustomMaterial, // weapon skins
    SsaoDepthWrite
};

class StudioRender {
    std::byte pad_0[0x250];
    Material* materialOverride;
    std::byte pad_1[0xC];
    OverrideType overrideType;
public:
    VIRTUAL_METHOD(void, forcedMaterialOverride, 33, (Material* material, OverrideType type = OverrideType::Normal, int index = -1), (this, material, type, index))

    bool isForcedMaterialOverride() noexcept
    {
        if (!materialOverride)
            return overrideType == OverrideType::DepthWrite || overrideType == OverrideType::SsaoDepthWrite; // see CStudioRenderContext::IsForcedMaterialOverride
        return std::string_view{ materialOverride->getName() }.starts_with("dev/glow");
    }
};
