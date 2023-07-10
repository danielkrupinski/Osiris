#pragma once

#include "Hooks/MinHook.h"
#include "Hooks/VmtSwap.h"
#include "Platform/Macros/IsPlatform.h"

#if IS_WIN32()
using HookType = MinHook;
#else
using HookType = VmtSwap;
#endif
