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

    int getInt()
    {
        using OriginalFn = int(__thiscall *)(void *);
        return callVirtualFunction<OriginalFn>(this, 13)(this);
    }

    void SetValue(const char *value)
    {
        using OriginalFn = void(__thiscall *)(void *, const char *);
        callVirtualFunction<OriginalFn>(this, 14)(this, value);
    }

    void SetValue(float value)
    {
        using OriginalFn = void(__thiscall *)(void *, float);
        callVirtualFunction<OriginalFn>(this, 15)(this, value);
    }

    void SetValue(int value)
    {
        using OriginalFn = void(__thiscall *)(void *, int);
        callVirtualFunction<OriginalFn>(this, 16)(this, value);
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
