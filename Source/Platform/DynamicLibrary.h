#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN32() || IS_WIN64()

#include "Windows/DynamicLibrary.h"

template <typename PlatformApi>
using DynamicLibrary = windows_platform::DynamicLibrary<PlatformApi>;

#elif IS_LINUX()

#include "Linux/LinuxDynamicLibrary.h"

template <typename PlatformApi>
using DynamicLibrary = LinuxDynamicLibrary<PlatformApi>;

#endif
