#pragma once

#include <Windows.h>

namespace windows_platform
{

struct PlatformApi {
    HMODULE GetModuleHandleA(LPCSTR moduleName) const noexcept
    {
        return ::GetModuleHandleA(moduleName);
    }
};

}
