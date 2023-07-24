#pragma once

#include <cassert>
#include <cstddef>
#include <limits>

namespace utils
{

template <std::size_t Size, std::size_t Alignment>
consteval std::size_t align() noexcept
{
    if constexpr (constexpr auto offset = Alignment - Size % Alignment; offset != Alignment) {
        static_assert(Size <= (std::numeric_limits<std::size_t>::max)() - offset, "Size is too large to align");
        return Size + offset;
    }
    return Size;
}

[[nodiscard]] inline std::size_t align(std::size_t size, std::size_t alignment) noexcept
{
    if (const auto offset = alignment - size % alignment; offset != alignment) {
        assert(size <= (std::numeric_limits<std::size_t>::max)() - offset && "Size is too large to align");
        return size + offset;
    }
    return size;
}

}
