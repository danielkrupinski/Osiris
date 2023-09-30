#pragma once

#include <cassert>
#include <emmintrin.h>
#include <span>
#include <string_view>

#include "BytePattern.h"
#include <emmintrin.h>

#include <Helpers/Bits.h>

class PatternFinderSIMD {
public:
    PatternFinderSIMD(std::span<const std::byte> bytes, BytePattern pattern)
        : bytes{ bytes }, pattern{ pattern }
    {

    }

    const std::byte* operator()() noexcept
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

    [[nodiscard]] std::span<const std::byte> getNotCheckedBytes() const noexcept
    {
        return bytes.subspan(currentPos);
    }

private:
    [[nodiscard]] std::size_t remainingBytes() const noexcept
    {
        return bytes.size() - currentPos;
    }

    [[nodiscard]] std::size_t indexOfLastPatternChar() const noexcept
    {
        return pattern.length() - 1;
    }

    [[nodiscard]] std::size_t byteSpanPerIteration() const noexcept
    {
        return indexOfLastPatternChar() + sizeof(__m128i);
    }

    [[nodiscard]] bool canDoAnotherIteration() const noexcept
    {
        return remainingBytes() >= byteSpanPerIteration();
    }

    std::size_t currentPos = 0;
    std::span<const std::byte> bytes;
    BytePattern pattern;
};
