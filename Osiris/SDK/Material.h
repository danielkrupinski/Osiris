#pragma once

#include "Utils.h"

enum class MaterialVar {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28
};

class Material final {
public:
    constexpr auto getName() noexcept
    {
        return std::string_view{ callVirtualFunction<const char*, void*>(this, 0)(this) };
    }

    constexpr int incrementReferenceCount() noexcept
    {
        callVirtualFunction<void, void*>(this, 12)(this);
        return 0;
    }

    constexpr void alphaModulate(float alpha) noexcept
    {
        callVirtualFunction<void, void*, float>(this, 27)(this, alpha);
    }

    constexpr void colorModulate(float color[3]) noexcept
    {
        callVirtualFunction<void, void*, float, float, float>(this, 28)(this, color[0], color[1], color[2]);
    }

    constexpr void setMaterialVarFlag(MaterialVar flag, bool on) noexcept
    {
        callVirtualFunction<void, void*, MaterialVar, bool>(this, 29)(this, flag, on);
    }
};
