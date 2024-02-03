#pragma once

#include <cstddef>
#include <span>
#include <string_view>

#include "BytePattern.h"
#include <Helpers/PatternNotFoundLogger.h>
#include "HybridPatternFinder.h"
#include <MemorySearch/PatternSearchResult.h>
#include <Utils/SpanSlice.h>

#include <Platform/Macros/FunctionAttributes.h>

enum class OffsetHint : std::size_t {};

template <typename NotFoundHandler>
struct PatternFinder {
public:
    PatternFinder(std::span<const std::byte> bytes, NotFoundHandler notFoundHandler)
        : bytes{ bytes }, notFoundHandler{ notFoundHandler }
    {
    }

    [[nodiscard]] [[NOINLINE]] PatternSearchResult operator()(BytePattern pattern) const noexcept
    {
        const auto found = HybridPatternFinder{ bytes, pattern }.findNextOccurrence();
        if (!found)
            notFoundHandler(pattern, bytes);
        return PatternSearchResult{ found };
    }

    [[nodiscard]] [[NOINLINE]] PatternSearchResult operator()(BytePattern pattern, OffsetHint offsetHint) const noexcept
    {
        const auto foundWithHint = HybridPatternFinder{ getSliceForHint(offsetHint), pattern }.findNextOccurrence();
        if (foundWithHint)
            return PatternSearchResult{ foundWithHint };
        return operator()(pattern);
    }

private:
    [[nodiscard]] std::span<const std::byte> getSliceForHint(OffsetHint offsetHint) const noexcept
    {
        return SpanSlice<20'000, const std::byte>{ bytes, static_cast<std::size_t>(offsetHint) }();
    }

    std::span<const std::byte> bytes;
    NotFoundHandler notFoundHandler;
};
