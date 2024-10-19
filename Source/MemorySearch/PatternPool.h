#pragma once

#include <array>
#include <numeric>
#include <limits>
#include <Utils/TypeList.h>

#include "PatternPoolBuilder.h"

template <typename StrongTypeAlias>
struct UnpackStrongTypeAlias {
    using type = typename StrongTypeAlias::Type;
};

template <typename T>
struct SizeOf {
    static constexpr auto value = sizeof(T);
};

template <typename StrongTypeAlias>
struct UnpackedStrongTypeAliasSizeOf {
    static constexpr auto value = sizeof(typename StrongTypeAlias::Type);
};

template <std::size_t N>
struct WithSizeOf {
    template <typename T>
    struct Equal {
        static constexpr auto value = (sizeof(T) == N);
    };

    template <typename T>
    struct LowerEqual {
        static constexpr auto value = (sizeof(T) <= N);
    };

    template <typename T>
    struct Greater {
        static constexpr auto value = (sizeof(T) > N);
    };
};

template <std::size_t BufferSize = 0, std::size_t NumberOfPatterns = 0, typename PatternTypesList = TypeList<>>
class PatternPool {
public:
    template <PatternPoolBuilder builder>
    [[nodiscard]] static consteval auto from() noexcept
    {
        using SortedPatternTypes = typename decltype(builder)::PatternTypes::template sortBy<UnpackedStrongTypeAliasSizeOf>;
        PatternPool<builder.tempPool.bufferSize, builder.tempPool.numberOfPatterns, SortedPatternTypes> pool;
        copyPatterns(builder.tempPool, pool, typename decltype(builder)::PatternTypes{}, SortedPatternTypes{});
        return pool;
    }

    using PatternTypes = PatternTypesList;

    template <typename F>
    void forEach(F f) const noexcept
    {
        std::size_t patternStartIndex = 0;
        for (std::size_t i = 0; i < NumberOfPatterns; ++i) {
            f(BytePattern{{buffer.data() + patternStartIndex, patternLengths[i]}, kPatternStringWildcard}, patternOffsets[i], operations[i]);
            patternStartIndex += patternLengths[i];
        }
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
