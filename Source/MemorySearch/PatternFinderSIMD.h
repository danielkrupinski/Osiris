#pragma once

#include <cassert>
#include <emmintrin.h>
#include <span>
#include <string_view>

#include "BytePattern.h"
#include "PatternFinder.h"

class PatternFinderSIMD {
public:
    PatternFinderSIMD(std::span<const std::byte> bytes, BytePattern pattern);

    const std::byte* operator()() noexcept;

    [[nodiscard]] std::span<const std::byte> getNotCheckedBytes() const noexcept
    {
        return bytes.subspan(currentPos);
    }

private:
    [[nodiscard]] std::size_t remainingBytes() const noexcept
    {
        return bytes.size() - currentPos;
    }

    [[nodiscard]] std::size_t indexOfLastPatternChar() const noexcept
    {
        return pattern.length() - 1;
    }

    [[nodiscard]] std::size_t byteSpanPerIteration() const noexcept
    {
        return indexOfLastPatternChar() + sizeof(__m128i);
    }

    [[nodiscard]] bool canDoAnotherIteration() const noexcept
    {
        return remainingBytes() >= byteSpanPerIteration();
    }

    std::size_t currentPos = 0;
    std::span<const std::byte> bytes;
    BytePattern pattern;
};
