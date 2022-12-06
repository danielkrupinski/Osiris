#pragma once

#include "VirtualMethod.h"

class KeyValues;
class RenderContext;

namespace csgo::pod
{
    struct Material;
    struct MaterialSystem;
}

class MaterialSystem : public VirtualCallableFromPOD<MaterialSystem, csgo::pod::MaterialSystem> {
public:
    VIRTUAL_METHOD(csgo::pod::Material*, createMaterial, 83, (const char* materialName, KeyValues* keyValues), (materialName, keyValues))
    VIRTUAL_METHOD(csgo::pod::Material*, findMaterial, 84, (const char* materialName, const char* textureGroupName = nullptr, bool complain = true, const char* complainPrefix = nullptr), (materialName, textureGroupName, complain, complainPrefix))
    VIRTUAL_METHOD(short, firstMaterial, 86, (), ())
    VIRTUAL_METHOD(short, nextMaterial, 87, (short handle), (handle))
    VIRTUAL_METHOD(short, invalidMaterial, 88, (), ())
    VIRTUAL_METHOD(csgo::pod::Material*, getMaterial, 89, (short handle), (handle))
    VIRTUAL_METHOD(RenderContext*, getRenderContext, 115, (), ())
};
