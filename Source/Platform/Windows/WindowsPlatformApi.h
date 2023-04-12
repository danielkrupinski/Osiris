#pragma once

#include <intrin.h>

#include "Win.h"

struct WindowsPlatformApi {
    [[nodiscard]] static win::Peb* getPeb() noexcept
    {
#if IS_WIN32()
        return reinterpret_cast<win::Peb*>(__readfsdword(0x30));
#elif IS_WIN64()
        return reinterpret_cast<win::Peb*>(__readgsqword(0x60));
#else
#error "Unsupported platform"
#endif
    }
};
