#pragma once

#include <type_traits>

#include "UtlVector.h"
#include "VirtualMethod.h"
#define FORCEINLINE_CVAR inline
#define FCVAR_NEVER_AS_STRING           (1<<12) // never try to print that cvar

ConVar* m_pParent;
int m_nFlags;

struct ConVar {
    VIRTUAL_METHOD(float, getFloat, 12, (), (this))
    VIRTUAL_METHOD(int, getInt, 13, (), (this))
    VIRTUAL_METHOD(void, setValue, 14, (const char* value), (this, value))
    VIRTUAL_METHOD(void, setValue, 15, (float value), (this, value))
    VIRTUAL_METHOD(void, setValue, 16, (int value), (this, value))
    FORCEINLINE_CVAR char const* GetString(void) const;
    
    template <typename T> T Get(void) const;
    template <> FORCEINLINE_CVAR const char* Get<const char*>(void) const { return GetString(); }
    std::byte pad[24];
    std::add_pointer_t<void __cdecl()> changeCallback;
    ConVar* parent;
    const char* defaultValue;
    char* string;
    std::byte pad1[28];
    UtlVector<void __cdecl()> onChangeCallbacks;
};

FORCEINLINE_CVAR const char *ConVar::GetString(void) const
{
    if (m_nFlags & FCVAR_NEVER_AS_STRING)
        return "FCVAR_NEVER_AS_STRING";
    char const* str = m_pParent->m_Value.m_pszString;
    return str ? str : "";
}