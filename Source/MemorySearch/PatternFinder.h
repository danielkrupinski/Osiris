#pragma once

#include <span>
#include <string_view>

#include "BytePattern.h"

enum class OffsetHint : std::size_t {};

struct PatternFinder {
public:
    explicit PatternFinder(std::span<const std::byte> bytes)
        : bytes{ bytes }
    {
    }

    [[nodiscard]] const std::byte* operator()(BytePattern pattern) const noexcept;
    [[nodiscard]] const std::byte* operator()(BytePattern pattern, OffsetHint offsetHint) const noexcept;

private:
    std::span<const std::byte> bytes;
};
