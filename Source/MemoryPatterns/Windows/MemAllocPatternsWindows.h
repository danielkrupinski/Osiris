#pragma once

#include <cstdint>

#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct MemAllocPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] std::int8_t* allocOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("FF 50 ? 48 89 43 ? 4C 8D"_pat).add(2).template as<std::int8_t*>();
    }
};
