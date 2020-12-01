#pragma once

#include <type_traits>

#include "Platform.h"
#include "UtlVector.h"
#include "VirtualMethod.h"

struct ConVar {
#ifdef _WIN32
    VIRTUAL_METHOD(float, getFloat, 12, (), (this))
    VIRTUAL_METHOD(int, getInt, 13, (), (this))
    VIRTUAL_METHOD(void, setValue, 14, (const char* value), (this, value))
    VIRTUAL_METHOD(void, setValue, 15, (float value), (this, value))
    VIRTUAL_METHOD(void, setValue, 16, (int value), (this, value))
#else
    VIRTUAL_METHOD(float, getFloat, 15, (), (this))
    VIRTUAL_METHOD(int, getInt, 16, (), (this))
    VIRTUAL_METHOD(void, setValue, 17, (const char* value), (this, value))
    VIRTUAL_METHOD(void, setValue, 18, (float value), (this, value))
    VIRTUAL_METHOD(void, setValue, 19, (int value), (this, value))
#endif

#ifdef _WIN32
    std::byte pad[24];
#else
    std::byte pad[48];
#endif
    std::add_pointer_t<void __CDECL()> changeCallback;
    ConVar* parent;
    const char* defaultValue;
    char* string;
    std::byte pad1[28];
    UtlVector<void(__CDECL*)()> onChangeCallbacks;
};
