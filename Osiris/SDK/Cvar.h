#pragma once

#include "Utils.h"

class Cvar {
public:
    constexpr std::string_view getString() noexcept
    {
        return callVirtualFunction<const char*(__thiscall*)(void*)>(this, 11)(this);
    }

    constexpr float getFloat() noexcept
    {
        return callVirtualFunction<float(__thiscall*)(void*)>(this, 12)(this);
    }

    constexpr int getInt() noexcept
    {
        return callVirtualFunction<int(__thiscall*)(void*)>(this, 13)(this);
    }

    constexpr void setValue(const std::string_view value) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const char*)>(this, 14)(this, value.data());
    }

    constexpr void setValue(float value) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, float)>(this, 15)(this, value);
    }

    constexpr void setValue(int value) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, int)>(this, 16)(this, value);
    }

    constexpr Cvar* findVar(const std::string_view name) noexcept
    {
        return callVirtualFunction<Cvar*(__thiscall*)(void*, const char*)>(this, 15)(this, name.data());
    }

    char pad_0x0000[0x4];
    Cvar *pNext;
    int32_t bRegistered;
    char *pszName;
    char *pszHelpString;
    int32_t nFlags;
    char pad_0x0018[0x4];
    Cvar *pParent;
    char *pszDefaultValue;
    char *strString;
    int32_t StringLength;
    float fValue;
    int32_t nValue;
    int32_t bHasMin;
    float fMinVal;
    int32_t bHasMax;
    float fMaxVal;
    void *fnChangeCallback;
};
