#include <bit>
#include <cassert>

#include "HybridPatternFinder.h"
#include "PatternFinder.h"
#include <Utils/SpanSlice.h>

const std::byte* PatternFinder::operator()(BytePattern pattern) const noexcept
{
    return HybridPatternFinder{ bytes, pattern }();
}

const std::byte* PatternFinder::operator()(BytePattern pattern, OffsetHint offsetHint) const noexcept
{
    const auto foundWithHint = HybridPatternFinder{ SpanSlice<20'000, const std::byte>{ bytes, static_cast<std::size_t>(offsetHint) }(), pattern }();
    if (foundWithHint)
        return foundWithHint;
    return HybridPatternFinder{ bytes, pattern }();
}
