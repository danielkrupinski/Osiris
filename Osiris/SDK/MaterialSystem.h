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

    constexpr auto firstMaterial() noexcept
    {
        return callVirtualMethod<short>(this, 86);
    }

    constexpr auto nextMaterial(short handle) noexcept
    {
        return callVirtualMethod<short, short>(this, 87, handle);
    }

    constexpr auto invalidMaterial() noexcept
    {
        return callVirtualMethod<short>(this, 88);
    }

    constexpr auto getMaterial(short handle) noexcept
    {
        return callVirtualMethod<Material*, short>(this, 89, handle);
    }

    constexpr auto getRenderContext() noexcept
    {
        return callVirtualMethod<RenderContext*>(this, 115);
    }
};
