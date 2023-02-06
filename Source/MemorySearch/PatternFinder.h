#pragma once

#include <cstddef>
#include <span>
#include <string_view>

#include "BytePattern.h"
#include <Utils/SafeAddress.h>

enum class OffsetHint : std::size_t {};

struct PatternFinder {
public:
    explicit PatternFinder(std::span<const std::byte> bytes)
        : bytes{ bytes }
    {
    }

    [[nodiscard]] SafeAddress operator()(BytePattern pattern) const noexcept;
    [[nodiscard]] SafeAddress operator()(BytePattern pattern, OffsetHint offsetHint) const noexcept;

private:
    [[nodiscard]] std::span<const std::byte> getSliceForHint(OffsetHint offsetHint) const noexcept;

    std::span<const std::byte> bytes;
};
