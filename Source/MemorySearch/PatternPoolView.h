#pragma once

#include <cstddef>
#include <cstdint>

#include "BytePattern.h"
#include "CodePatternOperation.h"
#include "PatternStringWildcard.h"

struct PatternPoolView {
    void forEach(auto f) const noexcept
    {
        std::size_t patternStartIndex = 0;
        for (std::size_t i = 0; i < numberOfPatterns; ++i) {
            f(BytePattern{{buffer + patternStartIndex, patternLengths[i]}, kPatternStringWildcard}, patternOffsets[i], operations[i]);
            patternStartIndex += patternLengths[i];
        }
    }

    std::size_t numberOfPatterns;
    const char* buffer;
    const std::uint8_t* patternLengths;
    const std::uint8_t* patternOffsets;
    const CodePatternOperation* operations;
};
