#pragma once

#include "Material.h"
#include "Utils.h"

class MaterialSystem {
public:
    constexpr auto findMaterial(const char* materialName) noexcept
    {
        return callVirtualFunction<Material*(__thiscall*)(void*, const char*, const char*, bool, const char*)>(this, 84)(this, materialName, nullptr, true, nullptr);
    }
};
