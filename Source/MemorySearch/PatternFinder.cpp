#include <bit>
#include <cassert>

#include "HybridPatternFinder.h"
#include "PatternFinder.h"
#include <Utils/SpanSlice.h>

SafeAddress PatternFinder::operator()(BytePattern pattern) const noexcept
{
    return SafeAddress{ std::uintptr_t(HybridPatternFinder{ bytes, pattern }()) };
}

SafeAddress PatternFinder::operator()(BytePattern pattern, OffsetHint offsetHint) const noexcept
{
    const auto foundWithHint = HybridPatternFinder{ getSliceForHint(offsetHint), pattern }();
    if (foundWithHint)
        return SafeAddress{ std::uintptr_t(foundWithHint) };
    return SafeAddress{ std::uintptr_t(HybridPatternFinder{ bytes, pattern }()) };
}

std::span<const std::byte> PatternFinder::getSliceForHint(OffsetHint offsetHint) const noexcept
{
    return SpanSlice<20'000, const std::byte>{ bytes, static_cast<std::size_t>(offsetHint) }();
}
