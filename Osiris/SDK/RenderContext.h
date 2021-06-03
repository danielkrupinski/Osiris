#pragma once

#include <functional>

#include "Inconstructible.h"
#include "VirtualMethod.h"

class RenderContext {
public:
    INCONSTRUCTIBLE(RenderContext)

    VIRTUAL_METHOD(void, release, 1, (), (this))
    VIRTUAL_METHOD(void, beginRender, 2, (), (this))
    VIRTUAL_METHOD(void, endRender, 3, (), (this))
    VIRTUAL_METHOD(void, getViewport, 41, (int& x, int& y, int& width, int& height), (this, std::ref(x), std::ref(y), std::ref(width), std::ref(height)))
};
