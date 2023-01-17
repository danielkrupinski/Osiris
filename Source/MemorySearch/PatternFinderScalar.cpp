#include <cassert>

#include "PatternFinder.h"
#include "PatternFinderScalar.h"

PatternFinderScalar::PatternFinderScalar(std::span<const std::byte> bytes, BytePattern pattern)
    : bytes{ bytes }, pattern{ pattern }
{
}

const std::byte* PatternFinderScalar::operator()() noexcept
{
    while (remainingBytes() >= pattern.length()) {
        if (pattern.matches(bytes.subspan(currentPos, pattern.length())))
            return &bytes[currentPos];
        ++currentPos;
    }
    return nullptr;
}
