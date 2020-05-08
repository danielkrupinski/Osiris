#pragma once

#include "Utils.h"

class InputSystem {
public:
    constexpr void enableInput(bool enable) noexcept
    {
        callVirtualMethod<void, bool>(this, 11, enable);
    }

    constexpr bool isButtonDown(int buttonCode) noexcept
    {
        return callVirtualMethod<bool, int>(this, 15, buttonCode);
    }

    constexpr void resetInputState() noexcept
    {
        callVirtualMethod<void>(this, 39);
    }

    constexpr auto buttonCodeToString(int buttonCode) noexcept
    {
        return callVirtualMethod<const char*, int>(this, 40, buttonCode);
    }

    constexpr auto virtualKeyToButtonCode(int virtualKey) noexcept
    {
        if (virtualKey <= VK_XBUTTON2) {
            if (virtualKey > VK_CANCEL) virtualKey--;
            return virtualKey + 106;
        }
        return callVirtualMethod<int, int>(this, 45, virtualKey);
    }

    constexpr auto virtualKeyToString(int virtualKey) noexcept
    {
        return buttonCodeToString(virtualKeyToButtonCode(virtualKey));
    }
};
