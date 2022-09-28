#include <memory>

#ifdef _WIN32
#include <clocale>
#include <Windows.h>
#endif

#include "GlobalContext.h"
#include "Hooks.h"

#ifdef _WIN32

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
    if (!_CRT_INIT(moduleHandle, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH) {
        std::setlocale(LC_CTYPE, ".utf8");
        globalContext.emplace();
        hooks.emplace(moduleHandle);
    }
    return TRUE;
}

#else

void __attribute__((constructor)) DllEntryPoint()
{
    globalContext.emplace();
    hooks.emplace(Hooks{});
}

#endif
