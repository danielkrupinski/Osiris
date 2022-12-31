#include <bit>
#include <cassert>

#include "HybridPatternFinder.h"
#include "PatternFinder.h"
#include <Utils/SpanSlice.h>

const std::byte* PatternFinder::operator()(std::string_view pattern) const noexcept
{
    assert(!pattern.empty());
    assert(pattern.front() != utils::wildcardChar && pattern.back() != utils::wildcardChar);

    return HybridPatternFinder{ bytes, pattern }();
}

const std::byte* PatternFinder::operator()(std::string_view pattern, OffsetHint offsetHint) const noexcept
{
    assert(!pattern.empty());
    assert(pattern.front() != utils::wildcardChar && pattern.back() != utils::wildcardChar);

    const auto foundWithHint = HybridPatternFinder{ SpanSlice<20'000, const std::byte>{ bytes, static_cast<std::size_t>(offsetHint) }(), pattern }();
    if (foundWithHint)
        return foundWithHint;
    return HybridPatternFinder{ bytes, pattern }();
}

bool matchPattern(std::span<const std::byte> bytes, std::string_view pattern) noexcept
{
    assert(bytes.size() == pattern.size());

    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (std::to_integer<char>(bytes[i]) != pattern[i] && pattern[i] != utils::wildcardChar)
            return false;
    }
    return true;
}
