#pragma once

#include <cstddef>
#include <span>
#include <string_view>

#include "BytePattern.h"

class HybridPatternFinder {
public:
    HybridPatternFinder(std::span<const std::byte> bytes, BytePattern pattern);

    [[nodiscard]] const std::byte* operator()() const noexcept;

private:
    std::span<const std::byte> bytes;
    BytePattern pattern;
};
