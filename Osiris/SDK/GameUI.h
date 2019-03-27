#pragma once

#include "Utils.h"

class GameUI {
public:
    constexpr void messageBox(const char* title, const char* text) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const char*, const char*, bool, bool, const char*, const char*, const char*, const char*, void*)>(this, 20)(this, title, text, true, false, nullptr, nullptr, nullptr, nullptr, nullptr);
    }
};