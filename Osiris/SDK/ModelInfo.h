#pragma once

#include "Utils.h"

class ModelInfo final {
public:
    constexpr auto getModelIndex(const char* name) noexcept
    {
        return callVirtualFunction<int, void*, const char*>(this, 2)(this, name);
    }
};
