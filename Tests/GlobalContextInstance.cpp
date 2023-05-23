#include <optional>

#include <GlobalContext.h>

#if IS_WIN32() || IS_WIN64()
#include <Platform/Windows/WindowsPlatformApi.h>
using PlatformApi = WindowsPlatformApi;
#elif IS_LINUX()
#include <Platform/Linux/LinuxPlatformApi.h>
using PlatformApi = LinuxPlatformApi;
#endif

namespace
{
    std::optional<GlobalContext<PlatformApi>> globalContext;
}

#include "Endpoints.h"

#include "GlobalContext.cpp"
template class GlobalContext<PlatformApi>;
