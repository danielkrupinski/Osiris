#pragma once

#include "Utils.h"

class GameUI {
public:
    constexpr void messageBox(const char* title, const char* text) noexcept
    {
        callVirtualMethod<void, const char*, const char*, bool, bool, const char*, const char*, const char*, const char*, void*>(this, 20, title, text, true, false, nullptr, nullptr, nullptr, nullptr, nullptr);
    }
};