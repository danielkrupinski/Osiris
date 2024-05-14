#pragma once

#include "PlatformSpecific.h"

#define THISCALL_CONV WIN64_ONLY(__thiscall)
#define FASTCALL_CONV WIN64_ONLY(__fastcall)
#define CDECL_CONV    WIN64_ONLY(__cdecl)
#define STDCALL_CONV  WIN64_ONLY(__stdcall)
