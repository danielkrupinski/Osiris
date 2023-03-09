#include <emmintrin.h>

#include <Helpers/Bits.h>
#include "PatternFinderSIMD.h"

PatternFinderSIMD::PatternFinderSIMD(std::span<const std::byte> bytes, BytePattern pattern)
    : bytes{ bytes }, pattern{ pattern }
{
    
}

const std::byte* PatternFinderSIMD::operator()() noexcept
{
    // http://0x80.pl/articles/simd-strfind.html

    const auto patternWithoutFirstAndLastChar = pattern.withoutFirstAndLastChar();

    const auto firstCharMask = _mm_set1_epi8(pattern.front());
    const auto lastCharMask = _mm_set1_epi8(pattern.back());

    for (; canDoAnotherIteration(); currentPos += sizeof(__m128i)) {
        const auto possibleFirstChars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&bytes[currentPos]));
        const auto possibleLastChars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&bytes[currentPos + indexOfLastPatternChar()]));

        const auto firstCharMatchPositions = _mm_cmpeq_epi8(firstCharMask, possibleFirstChars);
        const auto lastCharMatchPositions = _mm_cmpeq_epi8(lastCharMask, possibleLastChars);

        auto mask = static_cast<std::uint16_t>(_mm_movemask_epi8(_mm_and_si128(firstCharMatchPositions, lastCharMatchPositions)));
        while (mask != 0) {
            if (const auto bitPos = bits::countrZero(mask); patternWithoutFirstAndLastChar.matches(bytes.subspan(currentPos + bitPos + 1, patternWithoutFirstAndLastChar.length()))) {
                return &bytes[currentPos + bitPos];
            }

            mask = mask & (mask - 1);
        }
    }
    return nullptr;
}
