#pragma once

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "Optional.h"

template <typename ValueType>
class OptionalPointee {
public:
    OptionalPointee() = default;

    explicit OptionalPointee(ValueType* pointer) noexcept
        : pointer{pointer}
    {
    }

    [[nodiscard]] ValueType* get() const noexcept
    {
        return pointer;
    }

    [[nodiscard]] Optional<ValueType> toOptional() const noexcept
    {
        if (pointer)
            return *pointer;
        return {};
    }

    [[nodiscard]] auto valueOr(const ValueType& defaultValue) const noexcept
    {
        if (pointer)
            return *pointer;
        return defaultValue;
    }

    void operator=(const ValueType& value) const noexcept
    {
        if (pointer)
            *pointer = value;
    }

    [[nodiscard]] auto operator[](std::size_t index) const
        requires (std::is_bounded_array_v<ValueType>)
    {
        using ElementType = std::remove_extent_t<ValueType>;
        if (pointer) {
            assert(index < std::extent_v<ValueType>);
            return Optional<ElementType>{(*pointer)[index]};
        }
        return Optional<ElementType>{};
    }

private:
    ValueType* pointer{nullptr};
};

template <>
class OptionalPointee<void> {
public:
    OptionalPointee() = default;

    explicit OptionalPointee(void* pointer) noexcept
        : pointer{pointer}
    {
    }

    [[nodiscard]] void* get() const noexcept
    {
        return pointer;
    }

private:
    void* pointer{nullptr};
};
