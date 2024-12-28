#pragma once

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

    [[nodiscard]] ValueType valueOr(const ValueType& defaultValue) const noexcept
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
