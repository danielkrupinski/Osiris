#pragma once

#include "Utils.h"

class Material;
class RenderContext;

class MaterialSystem {
public:
    constexpr auto findMaterial(const char* materialName) noexcept
    {
        return callVirtualMethod<Material*, const char*, const char*, bool, const char*>(this, 84, materialName, nullptr, true, nullptr);
    }

    constexpr auto getRenderContext() noexcept
    {
        return callVirtualMethod<RenderContext*>(this, 115);
    }
};
