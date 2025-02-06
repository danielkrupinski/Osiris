#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

#include "CodePattern.h"
#include "CodePatternOperation.h"

template <std::size_t BufferCapacity, std::size_t MaxNumberOfPatterns>
class TempPatternPool {
    void error(const char*);
public:
    template <CodePattern Pattern>
    consteval void addPattern() noexcept
    {
        static_assert(Pattern.storage.size <= (std::numeric_limits<std::uint8_t>::max)());

        if (BufferCapacity - bufferSize < Pattern.storage.size)
            error("Not enough space in pattern buffer");

        if (numberOfPatterns == MaxNumberOfPatterns)
            error("Reached max number of patterns");

        std::ranges::copy_n(Pattern.storage.pattern.begin(), Pattern.storage.size, buffer.begin() + bufferSize);
        bufferSize += Pattern.storage.size;

        patternLengths[numberOfPatterns] = static_cast<std::uint8_t>(Pattern.storage.size);
        patternOffsets[numberOfPatterns] = Pattern.offset;
        operations[numberOfPatterns] = Pattern.operation;
        ++numberOfPatterns;
    }

    std::array<char, BufferCapacity> buffer{};
    std::array<std::uint8_t, MaxNumberOfPatterns> patternLengths{};
    std::array<std::uint8_t, MaxNumberOfPatterns> patternOffsets{};
    std::array<CodePatternOperation, MaxNumberOfPatterns> operations{};
    std::size_t bufferSize{0};
    std::size_t numberOfPatterns{0};
};
