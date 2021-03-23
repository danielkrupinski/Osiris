#pragma once

#include <cstdint>

#include "Platform.h"

class HudChat {};

class ClientMode {
public:
    auto getHudChat() noexcept
    {
        return reinterpret_cast<HudChat**>(std::uintptr_t(this) + WIN32_LINUX(28, 48));
    }
};
