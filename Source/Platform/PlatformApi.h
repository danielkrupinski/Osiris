#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN32() || IS_WIN64()
#include "Windows/WindowsPlatformApi.h"
using PlatformApi = WindowsPlatformApi;
#elif IS_LINUX()
#include "Linux/PlatformApi.h"
using PlatformApi = linux_platform::PlatformApi;
#endif
