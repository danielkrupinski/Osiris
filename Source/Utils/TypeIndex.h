#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace utils
{

template <typename T, typename Tuple, std::size_t I = 0>
[[nodiscard]] consteval std::size_t typeIndex() noexcept
{
    static_assert(I < std::tuple_size_v<Tuple>, "T does not exist in Tuple");

    if constexpr (std::is_same_v<T, std::tuple_element_t<I, Tuple>>)
        return I;
    else
        return typeIndex<T, Tuple, I + 1>();
}

}
