#include <memory>

#include "Platform/Macros/IsPlatform.h"

#if IS_WIN32()
#include <clocale>
#include <Windows.h>
#endif

#include "GlobalContext.h"
#include "Hooks.h"

#if IS_WIN32() || IS_WIN64()
#include "Platform/Windows/WindowsPlatformApi.h"
using PlatformApi = WindowsPlatformApi;
#elif IS_LINUX()
#include "Platform/Linux/LinuxPlatformApi.h"
using PlatformApi = LinuxPlatformApi;
#endif

namespace
{
    std::optional<GlobalContext<PlatformApi>> globalContext;
}

#include "Endpoints.h"

void initializeGlobalContext()
{
    globalContext.emplace(PlatformApi{});
}

#include "GlobalContext.cpp"
template class GlobalContext<PlatformApi>;

#if IS_WIN32() || IS_WIN64()

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
    if (!_CRT_INIT(moduleHandle, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH) {
        std::setlocale(LC_CTYPE, ".utf8");
        initializeGlobalContext();
        hooks.emplace(moduleHandle, DynamicLibrary<PlatformApi>{ csgo::CLIENT_DLL }, DynamicLibrary<PlatformApi>{ csgo::ENGINE_DLL }, DynamicLibrary<PlatformApi>{ csgo::VSTDLIB_DLL }, DynamicLibrary<PlatformApi>{ csgo::VGUIMATSURFACE_DLL });
    }
    return TRUE;
}

#elif IS_LINUX()

void __attribute__((constructor)) DllEntryPoint()
{
    initializeGlobalContext();
    hooks.emplace(DynamicLibrary<PlatformApi>{ csgo::CLIENT_DLL }, DynamicLibrary<PlatformApi>{ csgo::ENGINE_DLL }, DynamicLibrary<PlatformApi>{ csgo::VSTDLIB_DLL }, DynamicLibrary<PlatformApi>{ csgo::VGUIMATSURFACE_DLL });
}

#endif
