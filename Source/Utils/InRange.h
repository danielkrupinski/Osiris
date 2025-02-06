#pragma once

#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <type_traits>

// workaround for clang < 18 not supporting float non-type template parameters
template <typename T>
struct BinaryRepresentationTemplateParameter {
    std::array<std::byte, sizeof(T)> binaryRepresentation;

    explicit(false) constexpr BinaryRepresentationTemplateParameter(T value) noexcept
        : binaryRepresentation{std::bit_cast<decltype(binaryRepresentation)>(value)}
    {
    }

    template <typename U>
    BinaryRepresentationTemplateParameter(U) = delete;

    constexpr operator T() const noexcept
    {
        return std::bit_cast<T>(binaryRepresentation);
    }
};

template <typename T, BinaryRepresentationTemplateParameter<T> min, BinaryRepresentationTemplateParameter<T> max>
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
