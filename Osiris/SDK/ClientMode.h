#pragma once

#include <cstdint>

#include "Platform.h"

class HudChat {
public:
    template <typename... Args>
    void printf(int filter, const char* fmt, Args... args) noexcept
    {
        (*reinterpret_cast<void(__CDECL***)(void*, int, const char*, ...)>(this))[WIN32_LINUX(26, 29)](this, filter, fmt, args...);
    }
};

class ClientMode {
public:
    auto getHudChat() noexcept
    {
        return *reinterpret_cast<HudChat**>(std::uintptr_t(this) + WIN32_LINUX(28, 48));
    }
};
