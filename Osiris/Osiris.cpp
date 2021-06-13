#include <memory>

#ifdef _WIN32
#include <clocale>
#include <Windows.h>
#endif

#include "Hooks.h"
#include "VMP/def.h"
#ifdef _WIN32

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
     VMP_ULTRA("DllMain");
    if (!_CRT_INIT(moduleHandle, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH) {
        VMP_ULTRA("OnDllAttach");
        std::setlocale(LC_CTYPE, ".utf8");
        hooks = std::make_unique<Hooks>(moduleHandle);
    }
    return TRUE;
    VMP_END;
}

#else

void __attribute__((constructor)) DllEntryPoint()
{
    VMP_ULTRA("DllMain");
    hooks = std::make_unique<Hooks>();
    VMP_END;
}

#endif
