#include <bit>
#include <cassert>
#include <emmintrin.h>

#include "HybridPatternFinder.h"
#include "PatternFinder.h"

HybridPatternFinder::HybridPatternFinder(std::span<const std::byte> bytes, std::string_view pattern)
    : bytes{ bytes }, pattern{ pattern }
{
    assert(!pattern.empty());
    assert(pattern.front() != utils::wildcardChar && pattern.back() != utils::wildcardChar);
}

const std::byte* HybridPatternFinder::findSIMD() noexcept
{
    // http://0x80.pl/articles/simd-strfind.html

    const auto indexOfLastPatternChar = pattern.size() - 1;
    const auto patternWithoutFirstAndLastChar = pattern.size() > 2 ? std::string_view{ pattern.data() + 1, pattern.size() - 2 } : std::string_view{};
    const auto byteSpanPerIteration = indexOfLastPatternChar + sizeof(__m128i);

    const auto firstCharMask = _mm_set1_epi8(pattern[0]);
    const auto lastCharMask = _mm_set1_epi8(pattern.back());

    for (; currentPos + byteSpanPerIteration < bytes.size(); currentPos += sizeof(__m128i)) {
        const auto possibleFirstChars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&bytes[currentPos]));
        const auto possibleLastChars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&bytes[currentPos + indexOfLastPatternChar]));

        const auto firstCharMatchPositions = _mm_cmpeq_epi8(firstCharMask, possibleFirstChars);
        const auto lastCharMatchPositions = _mm_cmpeq_epi8(lastCharMask, possibleLastChars);

        auto mask = static_cast<std::uint16_t>(_mm_movemask_epi8(_mm_and_si128(firstCharMatchPositions, lastCharMatchPositions)));
        while (mask != 0) {
            if (const auto bitPos = std::countr_zero(mask); matchPattern(bytes.subspan(currentPos + bitPos + 1, patternWithoutFirstAndLastChar.length()), patternWithoutFirstAndLastChar)) {
                return &bytes[currentPos + bitPos];
            }

            mask = mask & (mask - 1);
        }
    }
    return nullptr;
}

const std::byte* HybridPatternFinder::findScalar() noexcept
{
    while (remainingBytes() >= pattern.size()) {
        if (matchPattern(bytes.subspan(currentPos, pattern.size()), pattern))
            return &bytes[currentPos];
        ++currentPos;
    }
    return nullptr;
}
