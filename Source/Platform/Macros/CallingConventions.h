#pragma once

#include "PlatformSpecific.h"

#define THISCALL_CONV WIN32_ARGS(__thiscall)
#define FASTCALL_CONV WIN32_ARGS(__fastcall)
#define CDECL_CONV    WIN32_ARGS(__cdecl)
#define STDCALL_CONV  WIN32_ARGS(__stdcall)
