#include <cassert>

#include "HybridPatternFinder.h"
#include "PatternFinder.h"
#include "PatternFinderScalar.h"
#include "PatternFinderSIMD.h"

HybridPatternFinder::HybridPatternFinder(std::span<const std::byte> bytes, std::string_view pattern)
    : bytes{ bytes }, pattern{ pattern }
{
    assert(!pattern.empty());
    assert(pattern.front() != utils::wildcardChar && pattern.back() != utils::wildcardChar);
}

const std::byte* HybridPatternFinder::operator()() noexcept
{
    PatternFinderSIMD simdFinder{ bytes, pattern };
    if (const auto foundSIMD = simdFinder())
        return foundSIMD;
    return PatternFinderScalar{ simdFinder.getNotCheckedBytes(), pattern }();
}
