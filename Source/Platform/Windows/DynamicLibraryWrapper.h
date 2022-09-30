#pragma once

#include <Windows.h>

namespace windows_platform
{

struct DynamicLibraryWrapper {
    FARPROC GetProcAddress(HMODULE module, LPCSTR procName) const noexcept
    {
        return ::GetProcAddress(module, procName);
    }

    HMODULE GetModuleHandleA(LPCSTR moduleName) const noexcept
    {
        return ::GetModuleHandleA(moduleName);
    }
);

}
