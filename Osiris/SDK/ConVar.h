#pragma once

#include "Utils.h"
#include "UtlVector.h"

struct ConVar {
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

    std::byte pad[24];
    std::add_pointer_t<void()> changeCallback;
    ConVar* parent;
    const char* defaultValue;
    char* string;
    std::byte pad1[28];
    UtlVector<void()> onChangeCallbacks;
};
