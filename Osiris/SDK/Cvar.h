#pragma once

#include "Utils.h"

class Cvar {
public:
    constexpr auto getString() noexcept
    {
        return std::string_view{ callVirtualMethod<const char*>(this, 11) };
    }

    constexpr float getFloat() noexcept
    {
        return callVirtualMethod<float>(this, 12);
    }

    constexpr int getInt() noexcept
    {
        return callVirtualMethod<int>(this, 13);
    }

    constexpr void setValue(const char* value) noexcept
    {
        callVirtualMethod<void, const char*>(this, 14, value);
    }

    constexpr void setValue(float value) noexcept
    {
        callVirtualMethod<void, float>(this, 15, value);
    }

    constexpr void setValue(int value) noexcept
    {
        callVirtualMethod<void, int>(this, 16, value);
    }

    constexpr Cvar* findVar(const char* name) noexcept
    {
        return callVirtualMethod<Cvar*, const char*>(this, 15, name);
    }

    std::byte pad[68];
    void* onChangeCallback;
};
