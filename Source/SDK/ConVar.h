#pragma once

#include <Platform/PlatformSpecific.h>
#include "VirtualMethod.h"

namespace csgo::pod { struct ConVar; }

struct ConVar : public VirtualCallableFromPOD<ConVar, csgo::pod::ConVar> {
    VIRTUAL_METHOD(float, getFloat, WIN32_LINUX(12, 15), (), ())
    VIRTUAL_METHOD(int, getInt, WIN32_LINUX(13, 16), (), ())
    VIRTUAL_METHOD(void, setValue, WIN32_LINUX(14, 17), (const char* value), (value))
    VIRTUAL_METHOD(void, setValue, WIN32_LINUX(15, 18), (float value), (value))
    VIRTUAL_METHOD(void, setValue, WIN32_LINUX(16, 19), (int value), (value))
};
