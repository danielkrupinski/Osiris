#include <bit>
#include <cassert>

#include "HybridPatternFinder.h"
#include "PatternFinder.h"

const std::byte* PatternFinder::operator()(std::string_view pattern) const noexcept
{
    assert(!pattern.empty());
    assert(pattern.front() != utils::wildcardChar && pattern.back() != utils::wildcardChar);

    HybridPatternFinder finder{ bytes, pattern };
    if (const auto foundSIMD = finder.findSIMD())
        return foundSIMD;
    return finder.findScalar();
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
