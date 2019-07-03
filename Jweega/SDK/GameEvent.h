#pragma once

#include "Utils.h"

class GameEvent {
public:
    constexpr auto getName() noexcept
    {
        return callVirtualMethod<const char*>(this, 1);
    }

    constexpr auto getInt(const char* keyName) noexcept
    {
        return callVirtualMethod<int, const char*, int>(this, 6, keyName, 0);
    }

    constexpr auto getString(const char* keyName) noexcept
    {
        return callVirtualMethod<const char*, const char*, const char*>(this, 9, keyName, "");
    }

    constexpr auto setString(const char* keyName, const char* value) noexcept
    {
        callVirtualMethod<void, const char*, const char*>(this, 16, keyName, value);
    }
};
