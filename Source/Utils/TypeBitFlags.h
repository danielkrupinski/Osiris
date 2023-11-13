#pragma once

#include <cstdint>
#include <limits>

#include "TypeIndex.h"

template <typename... Types>
    requires (sizeof...(Types) < std::numeric_limits<std::uint8_t>::digits)
class TypeBitFlags {
public:
    template <typename T>
    constexpr void set() noexcept
    {
        flags |= toMask<T>();
    }

    template <typename T>
    constexpr void unset() noexcept
    {
        flags &= ~toMask<T>();
    }

    template <typename T>
    [[nodiscard]] constexpr bool has() const noexcept
    {
        return (flags & toMask<T>()) != 0;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
        return flags != 0;
    }

private:
    template <typename T>
    static constexpr auto toMask() noexcept
    {
        return 1 << utils::typeIndex<T, std::tuple<Types...>>();
    }

    std::uint8_t flags{0};
};
