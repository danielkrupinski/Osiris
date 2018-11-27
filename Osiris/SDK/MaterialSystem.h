#pragma once

#include "Material.h"
#include "Utils.h"

class MaterialSystem {
public:
    Material* findMaterial(std::string_view materialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = nullptr)
    {
        return callVirtualFunction<Material*(__thiscall*)(void*, const char*, const char*, bool, const char*)>(this, 84)(this, materialName.data(), pTextureGroupName, complain, pComplainPrefix);
    }
};
