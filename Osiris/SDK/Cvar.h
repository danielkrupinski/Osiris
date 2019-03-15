#pragma once

#include "Utils.h"

class Cvar {
public:
    constexpr auto getString() noexcept
    {
        return std::string_view{ callVirtualFunction<const char*(__thiscall*)(void*)>(this, 11)(this) };
    }

    constexpr float getFloat() noexcept
    {
        return callVirtualFunction<float(__thiscall*)(void*)>(this, 12)(this);
    }

    constexpr int getInt() noexcept
    {
        return callVirtualFunction<int(__thiscall*)(void*)>(this, 13)(this);
    }

    constexpr void setValue(const char* value) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const char*)>(this, 14)(this, value);
    }

    constexpr void setValue(float value) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, float)>(this, 15)(this, value);
    }

    constexpr void setValue(int value) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, int)>(this, 16)(this, value);
    }

    constexpr Cvar* findVar(const char* name) noexcept
    {
        return callVirtualFunction<Cvar*(__thiscall*)(void*, const char*)>(this, 15)(this, name);
    }

    std::byte pad[68];
    void* onChangeCallback;
};
