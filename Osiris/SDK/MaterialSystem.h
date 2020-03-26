#pragma once

#include "VirtualMethod.h"

class Material;
class RenderContext;

class MaterialSystem {
public:
    VIRTUAL_METHOD(Material*, findMaterial, 84, (const char* materialName), (this, materialName, nullptr, true, nullptr))
    VIRTUAL_METHOD(short, firstMaterial, 86, (), (this))
    VIRTUAL_METHOD(short, nextMaterial, 87, (short handle), (this, handle))
    VIRTUAL_METHOD(short, invalidMaterial, 88, (), (this))
    VIRTUAL_METHOD(Material*, getMaterial, 89, (short handle), (this, handle))
    VIRTUAL_METHOD(RenderContext*, getRenderContext, 115, (), (this))
};
