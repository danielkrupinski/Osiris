#pragma once

#include <tuple>

#include "VirtualMethod.h"

namespace csgo::pod
{
    struct Material;
    struct MaterialVar;
}

class MaterialVar : public VirtualCallableFromPOD<MaterialVar, csgo::pod::MaterialVar> {
public:
    VIRTUAL_METHOD(void, setValue, 4, (float value), (value))
    VIRTUAL_METHOD(void, setVectorValue, WIN32_LINUX(11, 12), (float x, float y, float z), (x, y, z))
    VIRTUAL_METHOD(void, setVecComponentValue, 26, (float value, int component), (value, component))
};

enum class MaterialVarFlag {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28
};

class Material : public VirtualCallableFromPOD<Material, csgo::pod::Material> {
public:
    VIRTUAL_METHOD(const char*, getName, 0, (), ())
    VIRTUAL_METHOD(const char*, getTextureGroupName, 1, (), ())
    VIRTUAL_METHOD(csgo::pod::MaterialVar*, findVar, 11, (const char* name, bool* found = nullptr, bool complain = true), (name, found, complain))
    VIRTUAL_METHOD(void, incrementReferenceCount, 12, (), ())
    VIRTUAL_METHOD(void, decrementReferenceCount, 13, (), ())
    VIRTUAL_METHOD(void, alphaModulate, 27, (float alpha), (alpha))
    VIRTUAL_METHOD(void, colorModulate, 28, (const std::array<float, 3>& color), (color[0], color[1], color[2]))
    VIRTUAL_METHOD(void, colorModulate, 28, (float r, float g, float b), (r, g, b))
    VIRTUAL_METHOD(void, colorModulate, 28, (const std::tuple<float, float, float>& color), (std::get<0>(color), std::get<1>(color), std::get<2>(color)))
    VIRTUAL_METHOD(void, setMaterialVarFlag, 29, (MaterialVarFlag flag, bool on), (flag, on))
    VIRTUAL_METHOD(int, getReferenceCount, 56, (), ())
    VIRTUAL_METHOD(bool, isPrecached, 70, (), ())
};
