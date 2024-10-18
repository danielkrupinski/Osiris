#pragma once

#include <array>
#include <numeric>
#include <limits>
#include <Utils/TypeList.h>

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
    using PatternTypes = PatternTypesList;

    [[nodiscard]] consteval auto addPatterns(auto f) const noexcept
    {
        return f(*this);
    }

    template <typename PatternType, CodePattern Pattern>
    [[nodiscard]] consteval auto addPattern() const noexcept
    {
        static_assert(Pattern.storage.size <= (std::numeric_limits<std::uint8_t>::max)());
        static_assert(sizeof(typename PatternType::Type) == 8 || Pattern.operation == CodePatternOperation::Read, "Incorrect result size, missing .read() in pattern declaration?");

        PatternPool<BufferSize + Pattern.storage.size, NumberOfPatterns + 1, typename PatternTypesList::template add<PatternType>> newPool;
        copyCurrentPool(newPool);
        newPool.appendPattern(Pattern);
        return newPool;
    }

    [[nodiscard]] consteval auto finalize() const noexcept
    {
        using SortedPatternTypes = typename PatternTypesList::template sortBy<UnpackedStrongTypeAliasSizeOf>;
        PatternPool<BufferSize, NumberOfPatterns, SortedPatternTypes> finalPool;
        copyPatterns(finalPool);
        return finalPool;
    }

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
    template <typename... Types>
    consteval void copyPatterns(PatternPool<BufferSize, NumberOfPatterns, TypeList<Types...>>& pool) const noexcept
    {
        std::size_t outPatternIndex{0}, outBufferIndex{0};
        (copyPattern<Types>(outPatternIndex, outBufferIndex, pool), ...);
    }

    template <typename PatternType>
    consteval void copyPattern(std::size_t& outIndex, std::size_t& outBuffer, auto& pool) const noexcept
    {
        constexpr auto patternIndex = PatternTypesList::template indexOf<PatternType>();
        const auto patternBufferIndex = std::accumulate(patternLengths.begin(), patternLengths.begin() + patternIndex, 0);
        std::ranges::copy_n(buffer.begin() + patternBufferIndex, patternLengths[patternIndex], pool.buffer.begin() + outBuffer);
        outBuffer += patternLengths[patternIndex];
        pool.patternLengths[outIndex] = patternLengths[patternIndex];
        pool.patternOffsets[outIndex] = patternOffsets[patternIndex];
        pool.operations[outIndex] = operations[patternIndex];
        ++outIndex;
    }

    consteval void appendPattern(auto pattern) noexcept
    {
        std::ranges::copy_n(pattern.storage.pattern.begin(), pattern.storage.size, buffer.end() - pattern.storage.size);
        patternLengths.back() = static_cast<std::uint8_t>(pattern.storage.size);
        patternOffsets.back() = pattern.offset;
        operations.back() = pattern.operation;
    }

    consteval void copyCurrentPool(auto& newPool) const noexcept
    {
        std::ranges::copy(buffer, newPool.buffer.begin());
        std::ranges::copy(patternLengths, newPool.patternLengths.begin());
        std::ranges::copy(patternOffsets, newPool.patternOffsets.begin());
        std::ranges::copy(operations, newPool.operations.begin());
    }

    template <std::size_t, std::size_t, typename>
    friend class PatternPool;

    std::array<char, BufferSize> buffer{};
    std::array<std::uint8_t, NumberOfPatterns> patternLengths{};
    std::array<std::uint8_t, NumberOfPatterns> patternOffsets{};
    std::array<CodePatternOperation, NumberOfPatterns> operations{};
};
