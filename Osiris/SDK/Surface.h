#pragma once

#include "Utils.h"

class Surface final {
public:
    constexpr void setDrawColor(int r, int g, int b, int a) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, int, int, int, int)>(this, 14)(this, r, g, b, a);
    }

    constexpr void drawFilledRect(int x0, int y0, int x1, int y1) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, int, int, int, int)>(this, 16)(this, x0, y0, x1, y1);
    }

    constexpr void setTextFont(unsigned long font) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, unsigned long)>(this, 23)(this, font);
    }

    constexpr void setTextColor(int r, int g, int b, int a) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, int, int, int, int)>(this, 24)(this, r, g, b, a);
    }

    constexpr void setTextPosition(int x, int y) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, int, int)>(this, 26)(this, x, y);
    }

    constexpr void printText(const wchar_t* text, int textLength, int drawType = 0) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, const wchar_t*, int, int)>(this, 28)(this, text, textLength, drawType);
    }
};
