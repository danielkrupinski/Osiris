#pragma once

#include "Hooks/MinHook.h"
#include "Hooks/VmtHook.h"
#include "Hooks/VmtSwap.h"
#include "Platform/Macros/IsPlatform.h"

#if IS_WIN32()
// Easily switch hooking method for all hooks, choose between MinHook/VmtHook/VmtSwap
using HookType = MinHook;
#else
using HookType = VmtSwap;
#endif
