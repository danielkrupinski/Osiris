#pragma once

class Surface {
public:
    constexpr void unlockCursor() noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 66)(this);
    }

    constexpr void LockCursor() noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 67)(this);
    }
};
