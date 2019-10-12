#pragma once

#include "Utils.h"

class Surface {
public:
    static constexpr unsigned font{ 0x1d }; // builtin font from vgui_spew_fonts

    constexpr void setDrawColor(int r, int g, int b, int a) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 15, r, g, b, a);
    }

    constexpr void setDrawColor(const float color[3], int a) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 15, static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255), static_cast<int>(color[2] * 255), a);
    }

    template <typename T>
    constexpr void drawFilledRect(T x0, T y0, T x1, T y1) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 16, static_cast<int>(x0), static_cast<int>(y0), static_cast<int>(x1), static_cast<int>(y1));
    }

    template <typename T>
    constexpr void drawOutlinedRect(T x0, T y0, T x1, T y1) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 18, static_cast<int>(x0), static_cast<int>(y0), static_cast<int>(x1), static_cast<int>(y1));
    }

    template <typename T>
    constexpr void drawLine(T x0, T y0, T x1, T y1) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 19, static_cast<int>(x0), static_cast<int>(y0), static_cast<int>(x1), static_cast<int>(y1));
    }

    constexpr void drawPolyLine(int* xs, int* ys, int pointCount) noexcept
    {
        callVirtualMethod<void, int*, int*, int>(this, 20, xs, ys, pointCount);
    }

    constexpr void setTextFont(unsigned font) noexcept
    {
        callVirtualMethod<void, unsigned>(this, 23, font);
    }

    template <typename T>
    constexpr void setTextColor(T r, T g, T b, T a) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 25, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a));
    }

    constexpr void setTextColor(const float color[3], int a) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 25, static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255), static_cast<int>(color[2] * 255), a);
    }

    template <typename T>
    constexpr void setTextPosition(T x, T y) noexcept
    {
        callVirtualMethod<void, int, int>(this, 26, static_cast<int>(x), static_cast<int>(y));
    }

    constexpr void printText(const std::wstring_view text, int drawType = 0) noexcept
    {
        callVirtualMethod<void, const wchar_t*, int, int>(this, 28, text.data(), text.length(), drawType);
    }

    constexpr auto getScreenSize() noexcept
    {
        int width{ }, height{ };
        callVirtualMethod<void, int&, int&>(this, 44, width, height);
        return std::make_pair(width, height);
    }

    constexpr void unlockCursor() noexcept
    {
        callVirtualMethod<void>(this, 66);
    }

    constexpr unsigned createFont() noexcept
    {
        return callVirtualMethod<unsigned>(this, 71);
    }

    constexpr bool setFontGlyphSet(unsigned font, const char* fontName, int tall, int weight, int blur, int scanlines, int flags, int rangeMin = 0, int rangeMax = 0) noexcept
    {
        return callVirtualMethod<bool, unsigned, const char*, int, int, int, int, int, int, int>(this, 72, font, fontName, tall, weight, blur, scanlines, flags, rangeMin, rangeMax);
    }

    constexpr auto getTextSize(unsigned font, const wchar_t* text) noexcept
    {
        int width{ }, height{ };
        callVirtualMethod<void, unsigned, const wchar_t*, int&, int&>(this, 79, font, text, width, height);
        return std::make_pair(width, height);
    }

    template <typename T>
    constexpr void drawOutlinedCircle(T x, T y, int r, int seg) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 103, static_cast<int>(x), static_cast<int>(y), r, seg);
    }

    template <typename T>
    void drawCircle(T x, T y, int startRadius, int radius) noexcept
    {
        int xs[360];
        int ys[360];

        for (int i = startRadius; i <= radius; i++) {
            for (int j = 0; j < 360; j++) {
                xs[j] = static_cast<int>(cosf(degreesToRadians(static_cast<float>(j))) * i + x);
                ys[j] = static_cast<int>(sinf(degreesToRadians(static_cast<float>(j))) * i + y);
            }
            interfaces.surface->drawPolyLine(xs, ys, 360);
        }
    }
};
