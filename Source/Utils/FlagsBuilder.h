#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

template <std::integral T, typename Flag>
class FlagsBuilder {
public:
    template <Flag flag>
    constexpr FlagsBuilder& set(bool b) noexcept
    {
        static_assert(std::in_range<T>(rawType<flag>()));
        if (b)
            flags |= static_cast<T>(flag);
        return *this;
    }

    [[nodiscard]] T get() noexcept
    {
        return flags;
    }

private:
    template <Flag flag>
    static constexpr auto rawType() noexcept
    {
        if constexpr (std::is_enum_v<Flag>)
            return static_cast<std::underlying_type_t<Flag>>(flag);
        else
            return flag;
    }

    T flags{};
};
