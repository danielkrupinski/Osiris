#pragma once

#include "VirtualMethod.h"

class KeyValues;

namespace csgo
{

class RenderContext;

struct MaterialPOD;
struct MaterialSystemPOD;

class MaterialSystem : public VirtualCallableFromPOD<MaterialSystem, MaterialSystemPOD> {
public:
    VIRTUAL_METHOD(MaterialPOD*, createMaterial, 83, (const char* materialName, KeyValues* keyValues), (materialName, keyValues))
    VIRTUAL_METHOD(MaterialPOD*, findMaterial, 84, (const char* materialName, const char* textureGroupName = nullptr, bool complain = true, const char* complainPrefix = nullptr), (materialName, textureGroupName, complain, complainPrefix))
    VIRTUAL_METHOD(short, firstMaterial, 86, (), ())
    VIRTUAL_METHOD(short, nextMaterial, 87, (short handle), (handle))
    VIRTUAL_METHOD(short, invalidMaterial, 88, (), ())
    VIRTUAL_METHOD(MaterialPOD*, getMaterial, 89, (short handle), (handle))
    VIRTUAL_METHOD(RenderContext*, getRenderContext, 115, (), ())
};

}
