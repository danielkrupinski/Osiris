#pragma once

#include "Utils.h"

#include <tuple>

class MaterialVar {
public:
    constexpr auto setValue(float value) noexcept
    {
        callVirtualMethod<void, float>(this, 4, value);
    }
    constexpr auto setVectorValue(float r, float g, float b) noexcept
    {
        callVirtualMethod<void, float, float, float>(this, 11, r, g, b);
    }
};

enum class MaterialVarFlag {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28
};

class Material {
public:
    constexpr auto getName() noexcept
    {
        return callVirtualMethod<const char*>(this, 0);
    }

    constexpr auto getTextureGroupName() noexcept
    {
        return callVirtualMethod<const char*>(this, 1);
    }

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

    constexpr void colorModulate(const float color[3]) noexcept
    {
        callVirtualMethod<void, float, float, float>(this, 28, color[0], color[1], color[2]);
    }

    constexpr void colorModulate(float r, float g, float b) noexcept
    {
        callVirtualMethod<void, float, float, float>(this, 28, r, g, b);
    }

    constexpr void colorModulate(const std::tuple<float, float, float>& color) noexcept
    {
        callVirtualMethod<void, float, float, float>(this, 28, std::get<0>(color), std::get<1>(color), std::get<2>(color));
    }

    constexpr void setMaterialVarFlag(MaterialVarFlag flag, bool on) noexcept
    {
        callVirtualMethod<void, MaterialVarFlag, bool>(this, 29, flag, on);
    }

    constexpr auto isPrecached() noexcept
    {
        return callVirtualMethod<bool>(this, 70);
    }
};
