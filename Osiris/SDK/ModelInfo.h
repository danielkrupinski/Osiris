#pragma once

#include <string_view>
#include "Utils.h"

class ModelInfo final {
public:
    constexpr auto getModelIndex(const std::string_view name) noexcept
    {
        return callVirtualFunction<int(__thiscall*)(void*, const char*)>(this, 2)(this, name.data());
    }
};
