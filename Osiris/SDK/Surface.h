#pragma once

class Surface {
public:
    constexpr void unlockCursor() noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 66)(this);
    }
};
