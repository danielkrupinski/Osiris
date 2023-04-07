#pragma once

#include <type_traits>

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>

#include <Utils/Pad.h>
#include <CSGO/UtlVector.h>

namespace csgo
{

struct ConVarPOD {
    PAD(WIN32_LINUX(24, 48));
    std::add_pointer_t<void CDECL_CONV()> changeCallback;
    ConVarPOD* parent;
    const char* defaultValue;
    char* string;
    PAD(WIN32_LINUX(44, 56));
    UtlVector<void(CDECL_CONV*)()> onChangeCallbacks;
};

}
