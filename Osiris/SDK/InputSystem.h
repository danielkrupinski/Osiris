#pragma once

#include "VirtualMethod.h"

class InputSystem {
public:
    VIRTUAL_METHOD(void, enableInput, 11, (bool enable), (this, enable))
    VIRTUAL_METHOD(bool, isButtonDown, 15, (int buttonCode), (this, buttonCode))
    VIRTUAL_METHOD(void, resetInputState, 39, (), (this))
    VIRTUAL_METHOD(const char*, buttonCodeToString, 40, (int buttonCode), (this, buttonCode))

    constexpr auto virtualKeyToButtonCode(int virtualKey) noexcept
    {
#ifdef _WIN32
        if (virtualKey <= VK_XBUTTON2) {
            if (virtualKey > VK_CANCEL) virtualKey--;
            return virtualKey + 106;
        }
#endif
        return VirtualMethod::call<int, 45>(this, virtualKey);
    }

    auto virtualKeyToString(int virtualKey) noexcept
    {
        return buttonCodeToString(virtualKeyToButtonCode(virtualKey));
    }
};
