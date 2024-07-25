#pragma once

#include <type_traits>

namespace utils
{

template <typename T>
[[nodiscard]] decltype(auto) lvalue(std::remove_reference_t<T>& t) noexcept
{
    if constexpr (std::is_rvalue_reference_v<T>)
        return static_cast<std::remove_reference_t<T>>(t);
    else
        return t;
}

}
