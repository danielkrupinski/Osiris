#pragma once

#include <Platform/CallingConventions.h>

namespace csgo
{

using R_LoadNamedSkys = bool FASTCALL_CONV (const char* skyname);
using SendClanTag = void FASTCALL_CONV (const char* tag, const char* name);
using SubmitReport = bool STDCALL_CONV (LINUX_ARGS(void*, ) const char* xuid, const char* report);

}
