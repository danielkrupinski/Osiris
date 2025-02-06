#pragma once

#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>

#include "TypeIndex.h"

template <typename... Types>
    requires (sizeof...(Types) <= std::numeric_limits<std::uint32_t>::digits)
class TypeBitFlags {
public:
    TypeBitFlags() = default;

    template <typename T>
    constexpr TypeBitFlags& set() noexcept
    {
        flags |= toMask<T>();
        return *this;
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

    constexpr TypeBitFlags& operator|=(TypeBitFlags other) noexcept
    {
        flags |= other.flags;
        return *this;
    }

    [[nodiscard]] constexpr friend TypeBitFlags operator|(TypeBitFlags a, TypeBitFlags b) noexcept
    {
        return TypeBitFlags{static_cast<UnderlyingType>(a.flags | b.flags)};
    }

    [[nodiscard]] constexpr TypeBitFlags difference(TypeBitFlags other) const noexcept
    {
        return TypeBitFlags{static_cast<UnderlyingType>(flags & ~other.flags)};
    }

    [[nodiscard]] constexpr bool contains(TypeBitFlags other) const noexcept
    {
        return (flags & other.flags) == other.flags;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
        return flags != 0;
    }

private:
    using UnderlyingType = 
        std::conditional_t<sizeof...(Types) <= std::numeric_limits<std::uint8_t>::digits,
            std::uint8_t,
            std::conditional_t<sizeof...(Types) <= std::numeric_limits<std::uint16_t>::digits,
                std::uint16_t,
                std::uint32_t
            >
        >;

    explicit constexpr TypeBitFlags(UnderlyingType flags) noexcept
        : flags{flags}
    {
    }

    template <typename T>
    static constexpr auto toMask() noexcept
    {
        return 1 << utils::typeIndex<T, std::tuple<Types...>>();
    }

    UnderlyingType flags{0};
};
