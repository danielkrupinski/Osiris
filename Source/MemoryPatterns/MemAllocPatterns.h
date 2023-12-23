#pragma once

#include <cstdint>

#include <CS2/Classes/IMemAlloc.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct MemAllocPatterns {
    [[nodiscard]] std::int8_t* allocOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
