#pragma once

#include "Utils.h"
#include "Vector.h"

enum class MaterialVar {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28
};

class Material final {
public:
    constexpr auto getName() noexcept
    {
        return std::string_view{ callVirtualMethod<const char*>(this, 0) };
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

    constexpr void colorModulate(Vector color) noexcept
    {
        callVirtualMethod<void, float, float, float>(this, 28, color.x, color.y, color.z);
    }

    constexpr void setMaterialVarFlag(MaterialVar flag, bool on) noexcept
    {
        callVirtualMethod<void, MaterialVar, bool>(this, 29, flag, on);
    }
};
