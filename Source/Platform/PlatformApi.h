#pragma once

#include "Macros/IsPlatform.h"

#if IS_WIN32()
#include "Windows/PlatformApi.h"
using PlatformApi = windows_platform::PlatformApi;
#elif IS_LINUX()
#include "Linux/PlatformApi.h"
using PlatformApi = linux_platform::PlatformApi;
#endif
