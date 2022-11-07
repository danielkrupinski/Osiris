#include <bit>
#include <cassert>
#include <emmintrin.h>

#include "PatternFinder.h"

const std::byte* PatternFinder::operator()(std::string_view pattern) const noexcept
{
    // http://0x80.pl/articles/simd-strfind.html

    assert(!pattern.empty());
    assert(pattern.front() != utils::wildcardChar && pattern.back() != utils::wildcardChar);

    const auto indexOfLastPatternChar = pattern.size() - 1;
    const auto patternWithoutFirstAndLastChar = pattern.substr(1, pattern.size() >= 2 ? pattern.size() - 2 : 0);
    const auto byteSpanPerIteration = indexOfLastPatternChar + sizeof(__m128i);

    const auto firstCharMask = _mm_set1_epi8(pattern[0]);
    const auto lastCharMask  = _mm_set1_epi8(pattern.back());

    std::size_t i = 0;
    for (; i + byteSpanPerIteration < bytes.size(); i += sizeof(__m128i)) {
        const auto possibleFirstChars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&bytes[i]));
        const auto possibleLastChars  = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&bytes[i + indexOfLastPatternChar]));

        const auto firstCharMatchPositions = _mm_cmpeq_epi8(firstCharMask, possibleFirstChars);
        const auto lastCharMatchPositions  = _mm_cmpeq_epi8(lastCharMask, possibleLastChars);

        auto mask = static_cast<std::uint16_t>(_mm_movemask_epi8(_mm_and_si128(firstCharMatchPositions, lastCharMatchPositions)));
        while (mask != 0) {
            if (const auto bitPos = std::countr_zero(mask); matchPattern(bytes.subspan(i + bitPos + 1, patternWithoutFirstAndLastChar.length()), patternWithoutFirstAndLastChar)) {
                return &bytes[i + bitPos];
            }

            mask = mask & (mask - 1);
        }
    }

    while (i + pattern.size() <= bytes.size()) {
        if (matchPattern(bytes.subspan(i, pattern.size()), pattern))
            return &bytes[i];
        ++i;
    }

    return nullptr;
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
