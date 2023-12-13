#pragma once

#include "BytePattern.h"

template <typename T>
struct BinaryBytePattern {
    [[nodiscard]] explicit(false) operator BytePattern() const noexcept
    {
        return BytePattern{{reinterpret_cast<const char*>(&value), sizeof(value)}};
    }

    T value;
};
