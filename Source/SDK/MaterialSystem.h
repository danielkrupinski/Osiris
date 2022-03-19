#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class KeyValues;
class Material;
class RenderContext;

class MaterialSystem {
public:
    INCONSTRUCTIBLE(MaterialSystem)

    VIRTUAL_METHOD(Material*, createMaterial, 83, (const char* materialName, KeyValues* keyValues), (this, materialName, keyValues))
    VIRTUAL_METHOD(Material*, findMaterial, 84, (const char* materialName, const char* textureGroupName = nullptr, bool complain = true, const char* complainPrefix = nullptr), (this, materialName, textureGroupName, complain, complainPrefix))
    VIRTUAL_METHOD(short, firstMaterial, 86, (), (this))
    VIRTUAL_METHOD(short, nextMaterial, 87, (short handle), (this, handle))
    VIRTUAL_METHOD(short, invalidMaterial, 88, (), (this))
    VIRTUAL_METHOD(Material*, getMaterial, 89, (short handle), (this, handle))
    VIRTUAL_METHOD(RenderContext*, getRenderContext, 115, (), (this))
};
