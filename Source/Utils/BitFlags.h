#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

template <typename Flag, std::integral StorageType>
    requires std::is_enum_v<Flag>
class BitFlags {
public:
    template <Flag flag>
    constexpr void set() noexcept
    {
        flags |= toMask<flag>();
    }

    template <Flag flag>
    constexpr void unset() noexcept
    {
        flags &= ~toMask<flag>();
    }

    template <Flag flag>
    [[nodiscard]] constexpr bool has() const noexcept
    {
        return (flags & toMask<flag>()) != 0;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
        return flags != 0;
    }

private:
    template <Flag flag>
    static constexpr auto toMask() noexcept
    {
        static_assert(static_cast<std::underlying_type_t<Flag>>(flag) >= 0);
        static_assert(static_cast<std::underlying_type_t<Flag>>(flag) < std::numeric_limits<StorageType>::digits);
        return 1 << static_cast<std::underlying_type_t<Flag>>(flag);
    }

    StorageType flags{0};
};
