#pragma once

#include <string_view>

#include "BytePattern.h"

template <typename T>
struct BinaryBytePattern {
    [[nodiscard]] explicit(false) operator BytePattern() const noexcept
    {
        return BytePattern{std::string_view{reinterpret_cast<const char*>(&value), sizeof(value)}};
    }

    T value;
};

template <typename T> BinaryBytePattern(T value) -> BinaryBytePattern<T>;
