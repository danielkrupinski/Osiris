#pragma once

#include "Material.h"
#include "Utils.h"

class MaterialSystem {
public:
    constexpr auto findMaterial(const char* materialName) noexcept
    {
        return callVirtualMethod<Material*, const char*, const char*, bool, const char*>(this, 84, materialName, nullptr, true, nullptr);
    }
};
