#pragma once

#include <type_traits>

#include "Pad.h"
#include "Platform.h"
#include "UtlVector.h"
#include "VirtualMethod.h"

namespace csgo::pod
{

struct ConVar {
    PAD(WIN32_LINUX(24, 48))
    std::add_pointer_t<void CDECL_CONV()> changeCallback;
    ConVar* parent;
    const char* defaultValue;
    char* string;
    PAD(28)
    UtlVector<void(CDECL_CONV*)()> onChangeCallbacks;
};

}

struct ConVar : public VirtualCallableFromPOD<ConVar, csgo::pod::ConVar> {
    VIRTUAL_METHOD2(float, getFloat, WIN32_LINUX(12, 15), (), ())
    VIRTUAL_METHOD2(int, getInt, WIN32_LINUX(13, 16), (), ())
    VIRTUAL_METHOD2(void, setValue, WIN32_LINUX(14, 17), (const char* value), (value))
    VIRTUAL_METHOD2(void, setValue, WIN32_LINUX(15, 18), (float value), (value))
    VIRTUAL_METHOD2(void, setValue, WIN32_LINUX(16, 19), (int value), (value))
};
