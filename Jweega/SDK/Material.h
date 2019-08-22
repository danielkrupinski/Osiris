#pragma once

#include "Utils.h"

class MaterialVar {
public:
    constexpr auto setValue(float value) noexcept
    {
        callVirtualMethod<void, float>(this, 4, value);
    }
};

enum class MaterialVarFlag {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28
};

class Material {
public:
    constexpr auto findVar(const char* name) noexcept
    {
        return callVirtualMethod<MaterialVar*, const char*, bool*, bool>(this, 11, name, nullptr, false);
    }

    constexpr int incrementReferenceCount() noexcept
    {
        callVirtualMethod<void>(this, 12);
        return 0;
    }

    constexpr void alphaModulate(float alpha) noexcept
    {
        callVirtualMethod<void, float>(this, 27, alpha);
    }

    constexpr void colorModulate(float color[3]) noexcept
    {
        callVirtualMethod<void, float, float, float>(this, 28, color[0], color[1], color[2]);
    }

    constexpr void colorModulate(float r, float g, float b) noexcept
    {
        callVirtualMethod<void, float, float, float>(this, 28, r, g, b);
    }

    constexpr void setMaterialVarFlag(MaterialVarFlag flag, bool on) noexcept
    {
        callVirtualMethod<void, MaterialVarFlag, bool>(this, 29, flag, on);
    }
};
