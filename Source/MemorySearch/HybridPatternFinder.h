#pragma once

#include <cstddef>
#include <span>
#include <string_view>

#include "BytePattern.h"
#include "PatternFinderScalar.h"
#include "PatternFinderSIMD.h"

class HybridPatternFinder {
public:
    HybridPatternFinder(std::span<const std::byte> bytes, BytePattern pattern)
        : bytes{ bytes }, pattern{ pattern }
    {
    }

    [[nodiscard]] const std::byte* operator()() const noexcept
    {
        PatternFinderSIMD simdFinder{ bytes, pattern };
        if (const auto foundSIMD = simdFinder())
            return foundSIMD;
        return PatternFinderScalar{ simdFinder.getNotCheckedBytes(), pattern }();
    }

private:
    std::span<const std::byte> bytes;
    BytePattern pattern;
};
