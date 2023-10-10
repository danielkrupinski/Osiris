#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN64()

#include "Windows/WindowsPlatformApi.h"

using PlatformApi = WindowsPlatformApi;

#elif IS_LINUX()

#include "Linux/LinuxPlatformApi.h"

using PlatformApi = LinuxPlatformApi;

#endif
