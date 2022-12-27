#include <cassert>

#include "PatternFinder.h"
#include "PatternFinderScalar.h"

PatternFinderScalar::PatternFinderScalar(std::span<const std::byte> bytes, std::string_view pattern)
    : bytes{ bytes }, pattern{ pattern }
{
    assert(!pattern.empty());
    assert(pattern.front() != utils::wildcardChar && pattern.back() != utils::wildcardChar);
}

const std::byte* PatternFinderScalar::operator()() noexcept
{
    while (remainingBytes() >= pattern.size()) {
        if (matchPattern(bytes.subspan(currentPos, pattern.size()), pattern))
            return &bytes[currentPos];
        ++currentPos;
    }
    return nullptr;
}
