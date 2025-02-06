#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>

#include <Utils/Meta.h>
#include <Utils/StrongTypeAlias.h>
#include <Utils/TypeList.h>

#include "CodePatternOperation.h"
#include "PatternPoolBuilder.h"
#include "PatternPoolView.h"

template <std::size_t BufferSize = 0, std::size_t NumberOfPatterns = 0, typename PatternTypesList = TypeList<>>
class PatternPool {
public:
    template <PatternPoolBuilder builder>
    [[nodiscard]] static consteval auto from() noexcept
    {
        using SortedPatternTypes = typename decltype(builder)::PatternTypes::template sortBy<Projected<UnpackStrongTypeAlias, SizeOf>::Value>;
        PatternPool<builder.tempPool.bufferSize, builder.tempPool.numberOfPatterns, SortedPatternTypes> pool;
        copyPatterns(builder.tempPool, pool, typename decltype(builder)::PatternTypes{}, SortedPatternTypes{});
        return pool;
    }

    using PatternTypes = PatternTypesList;

    [[nodiscard]] PatternPoolView getView() const noexcept
    {
        return {NumberOfPatterns, buffer.data(), patternLengths.data(), patternOffsets.data(), operations.data()};
    }

private:
    template <typename... SourceTypes, typename... DestTypes>
    static consteval void copyPatterns(const auto& tempPatternPool, auto& pool, TypeList<SourceTypes...>, TypeList<DestTypes...>) noexcept
    {
        std::size_t outPatternIndex{0}, outBufferIndex{0};
        (copyPattern(TypeList<SourceTypes...>::template indexOf<DestTypes>(), outPatternIndex, outBufferIndex, tempPatternPool, pool), ...);
    }

    static consteval void copyPattern(std::size_t patternIndex, std::size_t& outIndex, std::size_t& outBuffer, const auto& tempPool, auto& pool) noexcept
    {
        const auto patternBufferIndex = std::accumulate(tempPool.patternLengths.begin(), tempPool.patternLengths.begin() + patternIndex, 0);
        std::ranges::copy_n(tempPool.buffer.begin() + patternBufferIndex, tempPool.patternLengths[patternIndex], pool.buffer.begin() + outBuffer);
        outBuffer += tempPool.patternLengths[patternIndex];
        pool.patternLengths[outIndex] = tempPool.patternLengths[patternIndex];
        pool.patternOffsets[outIndex] = tempPool.patternOffsets[patternIndex];
        pool.operations[outIndex] = tempPool.operations[patternIndex];
        ++outIndex;
    }

    template <std::size_t, std::size_t, typename>
    friend class PatternPool;

    std::array<char, BufferSize> buffer{};
    std::array<std::uint8_t, NumberOfPatterns> patternLengths{};
    std::array<std::uint8_t, NumberOfPatterns> patternOffsets{};
    std::array<CodePatternOperation, NumberOfPatterns> operations{};
};
