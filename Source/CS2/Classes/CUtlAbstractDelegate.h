#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>

#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

#if IS_WIN64()

struct CUtlAbstractDelegate {
    template <typename F>
        requires (std::is_pointer_v<F>&& std::is_function_v<std::remove_pointer_t<F>>)
    CUtlAbstractDelegate(F f)
        : function{std::uintptr_t(f)}
    {
    }

    void* thisptr{};
    std::uintptr_t function{};
};
static_assert(sizeof(CUtlAbstractDelegate) == 16);

#elif IS_LINUX()

struct CUtlAbstractDelegate {
    template <typename F>
        requires (std::is_pointer_v<F>&& std::is_function_v<std::remove_pointer_t<F>>)
    CUtlAbstractDelegate(F f)
        : functionOrVirtualMethodOffset{std::uintptr_t(f)}
    {
        assert(isUsingFunctionPointer());
    }

    [[nodiscard]] bool isUsingFunctionPointer() const noexcept
    {
        return (functionOrVirtualMethodOffset & 1) == 0;
    }

    void* thisptr{};
    std::uintptr_t functionOrVirtualMethodOffset{};
    std::ptrdiff_t offsetToThis{};
};
static_assert(sizeof(CUtlAbstractDelegate) == 24);

#endif
}
