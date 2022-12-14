#include <memory>

#include "Platform/Macros/IsPlatform.h"

#if IS_WIN32()
#include <clocale>
#include <Windows.h>
#endif

#include "Endpoints.h"
#include "Hooks.h"

#if IS_WIN32()

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
    if (!_CRT_INIT(moduleHandle, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH) {
        std::setlocale(LC_CTYPE, ".utf8");
        initializeGlobalContext();
        hooks.emplace(moduleHandle);
    }
    return TRUE;
}

#else

void __attribute__((constructor)) DllEntryPoint()
{
    initializeGlobalContext();
    hooks.emplace(Hooks{});
}

#endif
