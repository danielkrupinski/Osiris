#pragma once

#include "Utils.h"

class Cvar {
public:
    const char* GetString()
    {
        using OriginalFn = const char *(__thiscall *)(void *);
        return callVirtualFunction<OriginalFn>(this, 11)(this);
    }

    float GetFloat()
    {
        using OriginalFn = float(__thiscall *)(void *);
        return callVirtualFunction<OriginalFn>(this, 12)(this);
    }

    int GetInt()
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

    Cvar* FindVar(const char* name)
    {
        return callVirtualFunction<Cvar*(__thiscall*)(void*, const char*)>(this, 15)(this, name);
    }

    void* RegisterConCommand(Cvar *pCommandBase)
    {
        typedef Cvar*(__thiscall* OriginalFn)(void*, Cvar*);
        return callVirtualFunction<OriginalFn>(this, 9)(this, pCommandBase);
    }

    void* UnregisterConCommand(Cvar *pCommandBase)
    {
        typedef Cvar*(__thiscall* OriginalFn)(void*, Cvar*);
        return callVirtualFunction<OriginalFn>(this, 10)(this, pCommandBase);
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
