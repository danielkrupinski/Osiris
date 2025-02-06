#pragma once

#include <cstddef>
#include <span>

#include <Platform/Macros/FunctionAttributes.h>

#include "BytePattern.h"
#include "PatternFinderScalar.h"
#include "PatternFinderSIMD.h"

class HybridPatternFinder {
public:
    HybridPatternFinder(std::span<const std::byte> bytes, BytePattern pattern) noexcept
        : bytes{ bytes }, pattern{ pattern }
    {
    }

    [[nodiscard]] [[NOINLINE]] const std::byte* findNextOccurrence() noexcept
    {
        PatternFinderSIMD simdFinder{bytes, pattern};
        if (const auto foundSIMD = simdFinder()) {
            updateRemainingBytes(foundSIMD);
            return foundSIMD;
        }
        
        if (const auto foundScalar{PatternFinderScalar{simdFinder.getNotCheckedBytes(), pattern}()}) {
            updateRemainingBytes(foundScalar);
            return foundScalar;
        }

        return nullptr;
    }

private:
    void updateRemainingBytes(const std::byte* foundPosition) noexcept
    {
        bytes = {foundPosition + 1, bytes.data() + bytes.size()};
    }

    std::span<const std::byte> bytes;
    BytePattern pattern;
};
