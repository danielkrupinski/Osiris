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

    constexpr operator T() const noexcept
    {
        return std::bit_cast<T>(binaryRepresentation);
    }
};

template <typename T, BinaryRepresentationTemplateParameter<T> min, BinaryRepresentationTemplateParameter<T> max>
    requires (static_cast<T>(min) < static_cast<T>(max))
struct InRange {
    static constexpr auto kMin{static_cast<T>(min)};
    static constexpr auto kMax{static_cast<T>(max)};

    using ValueType = T;

    explicit constexpr InRange(T value) noexcept
        : value{value}
    {
        if (std::is_constant_evaluated()) {
            if (value < kMin || value > kMax)
                fail("Value out of range");
        } else {
            assert(value >= kMin);
            assert(value <= kMax);
        }
    }

    [[nodiscard]] constexpr bool operator==(const InRange&) const = default;

    [[nodiscard]] constexpr operator const T&() const noexcept
    {
        return value;
    }

private:
    void fail(const char*) {}

    T value;
};

template <typename T>
struct IsRangeConstrained : std::false_type {};

template <typename T, BinaryRepresentationTemplateParameter<T> min, BinaryRepresentationTemplateParameter<T> max>
struct IsRangeConstrained<InRange<T, min, max>> : std::true_type {};
