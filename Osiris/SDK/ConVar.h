#pragma once

#include <type_traits>

#include "Inconstructible.h"
#include "Pad.h"
#include "Platform.h"
#include "UtlVector.h"
#include "VirtualMethod.h"

struct ConVar {
    INCONSTRUCTIBLE(ConVar)

    VIRTUAL_METHOD(float, getFloat, WIN32_LINUX(12, 15), (), (this))
    VIRTUAL_METHOD(int, getInt, WIN32_LINUX(13, 16), (), (this))
    VIRTUAL_METHOD(void, setValue, WIN32_LINUX(14, 17), (const char* value), (this, value))
    VIRTUAL_METHOD(void, setValue, WIN32_LINUX(15, 18), (float value), (this, value))
    VIRTUAL_METHOD(void, setValue, WIN32_LINUX(16, 19), (int value), (this, value))

    PAD(WIN32_LINUX(24, 48))
    std::add_pointer_t<void CDECL_CONV()> changeCallback;
    ConVar* parent;
    const char* defaultValue;
    char* string;
    PAD(28)
    UtlVector<void(CDECL_CONV*)()> onChangeCallbacks;
};
