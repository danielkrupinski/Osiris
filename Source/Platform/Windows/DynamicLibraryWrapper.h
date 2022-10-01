#pragma once

#include <Windows.h>

namespace windows_platform
{

struct DynamicLibraryWrapper {
    FARPROC GetProcAddress(HMODULE moduleHandle, LPCSTR procName) const noexcept
    {
        return ::GetProcAddress(moduleHandle, procName);
    }

    HMODULE GetModuleHandleA(LPCSTR moduleName) const noexcept
    {
        return ::GetModuleHandleA(moduleName);
    }
};

}
