#pragma once

#include <cstddef>
#include <span>

#include "BytePattern.h"

class PatternFinderScalar {
public:
    PatternFinderScalar(std::span<const std::byte> bytes, BytePattern pattern)
        : bytes{ bytes }, pattern{ pattern }
    {
    }

    const std::byte* operator()() noexcept
    {
        while (remainingBytes() >= pattern.length()) {
            if (pattern.matches(bytes.subspan(currentPos, pattern.length())))
                return &bytes[currentPos];
            ++currentPos;
        }
        return nullptr;
    }

private:
    [[nodiscard]] std::size_t remainingBytes() const noexcept
    {
        return bytes.size() - currentPos;
    }

    std::size_t currentPos = 0;
    std::span<const std::byte> bytes;
    BytePattern pattern;
};

