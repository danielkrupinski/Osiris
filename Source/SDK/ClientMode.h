#pragma once

#include <cstdint>

#include "Inconstructible.h"
#include "Platform.h"

class HudChat {
public:
    INCONSTRUCTIBLE(HudChat)

    template <typename... Args>
    void printf(int filter, const char* fmt, Args... args) noexcept
    {
        (*reinterpret_cast<void(CDECL_CONV***)(void*, int, const char*, ...)>(this))[WIN32_LINUX(26, 29)](this, filter, fmt, args...);
    }
};

class ClientMode {
public:
    INCONSTRUCTIBLE(ClientMode)

    auto getHudChat() noexcept
    {
        return *reinterpret_cast<HudChat**>(std::uintptr_t(this) + WIN32_LINUX(28, 48));
    }
};
