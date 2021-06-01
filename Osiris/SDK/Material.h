#pragma once

#include <tuple>

#include "Inconstructible.h"
#include "VirtualMethod.h"

class MaterialVar {
public:
    INCONSTRUCTIBLE(MaterialVar)

    VIRTUAL_METHOD(void, setValue, 4, (float value), (this, value))
    VIRTUAL_METHOD(void, setVectorValue, WIN32_LINUX(11, 12), (float x, float y, float z), (this, x, y, z))
    VIRTUAL_METHOD(void, setVecComponentValue, 26, (float value, int component), (this, value, component))
};

enum class MaterialVarFlag {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28
};

class Material {
public:
    INCONSTRUCTIBLE(Material)

    VIRTUAL_METHOD(const char*, getName, 0, (), (this))
    VIRTUAL_METHOD(const char*, getTextureGroupName, 1, (), (this))
    VIRTUAL_METHOD(MaterialVar*, findVar, 11, (const char* name, bool* found = nullptr, bool complain = true), (this, name, found, complain))
    VIRTUAL_METHOD(void, incrementReferenceCount, 12, (), (this))
    VIRTUAL_METHOD(void, decrementReferenceCount, 13, (), (this))
    VIRTUAL_METHOD(void, alphaModulate, 27, (float alpha), (this, alpha))
    VIRTUAL_METHOD(void, colorModulate, 28, (const std::array<float, 3>& color), (this, color[0], color[1], color[2]))
    VIRTUAL_METHOD(void, colorModulate, 28, (float r, float g, float b), (this, r, g, b))
    VIRTUAL_METHOD(void, colorModulate, 28, (const std::tuple<float, float, float>& color), (this, std::get<0>(color), std::get<1>(color), std::get<2>(color)))
    VIRTUAL_METHOD(void, setMaterialVarFlag, 29, (MaterialVarFlag flag, bool on), (this, flag, on))
    VIRTUAL_METHOD(int, getReferenceCount, 56, (), (this))
    VIRTUAL_METHOD(bool, isPrecached, 70, (), (this))
};
