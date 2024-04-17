#pragma once

#include <cassert>

template <typename T, T min, T max>
    requires (min <= max)
struct InRange {
    explicit(false) constexpr InRange(T value) noexcept
        : value{value}
    {
        assert(value >= min);
        assert(value <= max);
        if (std::is_constant_evaluated()) {
            if (value < min || value > max)
                fail();
        }
    }

    constexpr operator T&() noexcept
    {
        return value;
    }

    constexpr operator const T&() const noexcept
    {
        return value;
    }

private:
    void fail() {}

    T value;
};
