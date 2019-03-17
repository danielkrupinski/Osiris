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
        return std::string_view{ callVirtualFunction<const char*(__thiscall*)(void*)>(this, 0)(this) };
    }

    constexpr int incrementReferenceCount() noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 12)(this);
        return 0;
    }

    constexpr void alphaModulate(float alpha) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, float)>(this, 27)(this, alpha);
    }

    constexpr void colorModulate(float color[3]) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, float, float, float)>(this, 28)(this, color[0], color[1], color[2]);
    }

    constexpr void setMaterialVarFlag(MaterialVar flag, bool on) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, MaterialVar, bool)>(this, 29)(this, flag, on);
    }
};
