#pragma once

#include "Material.h"
#include "Utils.h"

class MaterialSystem {
public:
    Material* findMaterial(const std::string& materialName)
    {
        return callVirtualFunction<Material*(__thiscall*)(void*, const char*, const char*, bool, const char*)>(this, 84)(this, materialName.c_str(), nullptr, true, nullptr);
    }
};
