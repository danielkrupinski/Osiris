#pragma once

#include "Material.h"
#include "Utils.h"

class MaterialSystem {
public:
    Material* findMaterial(const char* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = nullptr)
    {
        return callVirtualFunction<Material*(__thiscall*)(void*, const char*, const char*, bool, const char*)>(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
    }
};
