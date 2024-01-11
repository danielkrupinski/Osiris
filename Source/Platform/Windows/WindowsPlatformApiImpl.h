#pragma once

#include <bit>
#include <intrin.h>

#include <Platform/Macros/IsPlatform.h>

#include "WindowsPlatformApi.h"

win::Peb* WindowsPlatformApi::getPeb() noexcept
{
    static_assert(IS_WIN64());
    return std::bit_cast<win::Peb*>(__readgsqword(0x60));
}

void WindowsPlatformApi::debugBreak() noexcept
{
    __debugbreak();
}
