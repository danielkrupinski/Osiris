#include <cassert>

#include "HybridPatternFinder.h"
#include "PatternFinder.h"
#include "PatternFinderScalar.h"
#include "PatternFinderSIMD.h"

HybridPatternFinder::HybridPatternFinder(std::span<const std::byte> bytes, BytePattern pattern)
    : bytes{ bytes }, pattern{ pattern }
{
}

const std::byte* HybridPatternFinder::operator()() const noexcept
{
    PatternFinderSIMD simdFinder{ bytes, pattern };
    if (const auto foundSIMD = simdFinder())
        return foundSIMD;
    return PatternFinderScalar{ simdFinder.getNotCheckedBytes(), pattern }();
}
