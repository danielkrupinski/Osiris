#pragma once

#include "Hooks/MinHook.h"
#include "Platform/Macros/IsPlatform.h"
#include "Vmt/VmtSwapper.h"

#if IS_WIN32()
using HookType = MinHook;
#else
using HookType = VmtSwapper;
#endif
