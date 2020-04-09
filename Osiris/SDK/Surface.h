#pragma once

#include <functional>
#include <tuple>

#include "Utils.h"
#include "VirtualMethod.h"

class Surface {
public:
    static constexpr unsigned font{ 0x1d }; // builtin font from vgui_spew_fonts

    constexpr void setDrawColor(int r, int g, int b, int a = 255) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 15, r, g, b, a);
    }

    constexpr void setDrawColor(const float color[3], int a = 255) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 15, static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255), static_cast<int>(color[2] * 255), a);
    }

    constexpr void setDrawColor(std::tuple<float, float, float> color, int a = 255) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 15, static_cast<int>(std::get<0>(color) * 255), static_cast<int>(std::get<1>(color) * 255), static_cast<int>(std::get<2>(color) * 255), a);
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

    constexpr void setTextColor(int r, int g, int b, int a = 255) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 25, r, g, b, a);
    }

    constexpr void setTextColor(const float color[3], int a = 255) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 25, static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255), static_cast<int>(color[2] * 255), a);
    }

    constexpr void setTextColor(std::tuple<float, float, float> color, int a = 255) noexcept
    {
        callVirtualMethod<void, int, int, int, int>(this, 25, static_cast<int>(std::get<0>(color) * 255), static_cast<int>(std::get<1>(color) * 255), static_cast<int>(std::get<2>(color) * 255), a);
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
    
    VIRTUAL_METHOD(void, getScreenSize, 44, (int& w, int& h), (this, std::ref(w), std::ref(h)))

    auto getScreenSize() noexcept
    {
        int w, h;
        getScreenSize(w, h);
        return std::make_pair(w, h);
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
        int xs[12];
        int ys[12];

        for (int i = startRadius; i <= radius; ++i) {
            for (int j = 0; j < 12; ++j) {
                xs[j] = static_cast<int>(std::cos(degreesToRadians(static_cast<float>(j * 30))) * i + x);
                ys[j] = static_cast<int>(std::sin(degreesToRadians(static_cast<float>(j * 30))) * i + y);
            }
            interfaces->surface->drawPolyLine(xs, ys, 12);
        }
    }
};
