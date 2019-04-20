#pragma once

#include "Utils.h"

class Cvar {
public:
    constexpr auto getString() noexcept
    {
        return std::string_view{ callVirtualFunction<const char*>(this, 11) };
    }

    constexpr float getFloat() noexcept
    {
        return callVirtualFunction<float>(this, 12);
    }

    constexpr int getInt() noexcept
    {
        return callVirtualFunction<int>(this, 13);
    }

    constexpr void setValue(const char* value) noexcept
    {
        callVirtualFunction<void, const char*>(this, 14, value);
    }

    constexpr void setValue(float value) noexcept
    {
        callVirtualFunction<void, float>(this, 15, value);
    }

    constexpr void setValue(int value) noexcept
    {
        callVirtualFunction<void, int>(this, 16, value);
    }

    constexpr Cvar* findVar(const char* name) noexcept
    {
        return callVirtualFunction<Cvar*, const char*>(this, 15, name);
    }

    std::byte pad[68];
    void* onChangeCallback;
};
