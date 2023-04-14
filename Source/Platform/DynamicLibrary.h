#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN32() || IS_WIN64()

#include "Windows/WindowsDynamicLibrary.h"

template <typename PlatformApi>
using DynamicLibrary = WindowsDynamicLibrary<PlatformApi>;

#elif IS_LINUX()

#include "Linux/LinuxDynamicLibrary.h"

template <typename PlatformApi>
using DynamicLibrary = LinuxDynamicLibrary<PlatformApi>;

#endif
