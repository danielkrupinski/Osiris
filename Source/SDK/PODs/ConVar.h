#pragma once

#include <type_traits>

#include <Platform/CallingConventions.h>
#include <Platform/PlatformSpecific.h>

#include <SDK/Pad.h>
#include <SDK/UtlVector.h>

namespace csgo::pod
{

struct ConVar {
    PAD(WIN32_LINUX(24, 48))
    std::add_pointer_t<void CDECL_CONV()> changeCallback;
    ConVar* parent;
    const char* defaultValue;
    char* string;
    PAD(WIN32_LINUX(44, 56))
    UtlVector<void(CDECL_CONV*)()> onChangeCallbacks;
};

}
