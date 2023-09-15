#pragma once

#include <type_traits>

struct GenericFunctionPointer {
    template <typename FunctionPointer>
        requires std::is_pointer_v<FunctionPointer> && std::is_function_v<std::remove_pointer_t<FunctionPointer>>
    explicit(false) GenericFunctionPointer(FunctionPointer function) noexcept
        : value{ reinterpret_cast<void(*)()>(function) }
    {
    }

    template <typename FunctionPointer>
        requires std::is_pointer_v<FunctionPointer> /* && std::is_function_v<std::remove_pointer_t<FunctionPointer>> */ // doesn't work with __thiscall*, todo: uncomment when x64 only
    explicit(false) operator FunctionPointer() noexcept
    {
        return reinterpret_cast<FunctionPointer>(value);
    }

private:
    void (*value)();
};
