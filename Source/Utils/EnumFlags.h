#pragma once

#include <type_traits>
#include "ToUnderlying.h"

template <typename Enum>
    requires std::is_enum_v<Enum> && (!std::is_same_v<std::underlying_type_t<Enum>, bool>)
class EnumFlags {
public:
    [[nodiscard]] constexpr bool has(Enum flag) const noexcept
    {
        return (value & toUnderlying(flag)) != 0;
    }

    [[nodiscard]] constexpr bool hasAny() const noexcept
    {
        return value != 0;
    }

    constexpr void set(Enum flag) noexcept
    {
        value |= toUnderlying(flag);
    }

    constexpr void setAll() noexcept
    {
        value = ~0;
    }

    constexpr void clear() noexcept
    {
        value = 0;
    }

private:
    std::underlying_type_t<Enum> value{0};
};
