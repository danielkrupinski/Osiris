#pragma once

#include <tuple>
#include "Utils.h"

class RenderContext {
public:
    constexpr auto release() noexcept
    {
        callVirtualMethod<void>(this, 1);
    }

    constexpr auto beginRender() noexcept
    {
        callVirtualMethod<void>(this, 2);
    }

    constexpr auto endRender() noexcept
    {
        callVirtualMethod<void>(this, 3);
    }

    constexpr auto getViewport(int& x, int& y, int& width, int& height) noexcept
    {
        callVirtualMethod<void, int&, int&, int&, int&>(this, 41, x, y, width, height);
    }
};
