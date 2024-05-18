#pragma once

#include <cstdint>

#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct MemAllocPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] std::int8_t* allocOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("E6 06 FF 50 ? 48 89 43"_pat).add(4).template as<std::int8_t*>();
    }
};
