#pragma once

#include <array>
#include <numeric>
#include <limits>
#include <Utils/TypeList.h>

template <typename StrongTypeAlias>
struct UnpackStrongTypeAlias {
    using type = StrongTypeAlias::Type;
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

        using LeadingTypes = PatternTypesList::template filterTransformed<UnpackStrongTypeAlias, WithSizeOf<sizeof(typename PatternType::Type)>::template LowerEqual>;
        using TrailingTypes = PatternTypesList::template filterTransformed<UnpackStrongTypeAlias, WithSizeOf<sizeof(typename PatternType::Type)>::template Greater>;

        PatternPool<BufferSize + Pattern.storage.size, NumberOfPatterns + 1, typename LeadingTypes::template add<PatternType>::template concat<TrailingTypes>> newPool;

        copyCurrentPool(newPool);
        newPool.appendPattern(Pattern);

        std::rotate(newPool.patternLengths.begin() + LeadingTypes::size(), newPool.patternLengths.end() - 1, newPool.patternLengths.end());
        std::rotate(newPool.patternOffsets.begin() + LeadingTypes::size(), newPool.patternOffsets.end() - 1, newPool.patternOffsets.end());
        std::rotate(newPool.operations.begin() + LeadingTypes::size(), newPool.operations.end() - 1, newPool.operations.end());
        std::rotate(newPool.offsetsToNextInstruction.begin() + LeadingTypes::size(), newPool.offsetsToNextInstruction.end() - 1, newPool.offsetsToNextInstruction.end());

        const auto index = std::accumulate(patternLengths.begin(), patternLengths.begin() + LeadingTypes::size(), 0);
        std::rotate(newPool.buffer.begin() + index, newPool.buffer.end() - Pattern.storage.size, newPool.buffer.end());
        return newPool;
    }

    template <typename F>
    void forEach(F f) const noexcept
    {
        std::size_t patternStartIndex = 0;
        for (std::size_t i = 0; i < NumberOfPatterns; ++i) {
            f(BytePattern{{buffer.data() + patternStartIndex, patternLengths[i]}, kPatternStringWildcard}, patternOffsets[i], operations[i], offsetsToNextInstruction[i]);
            patternStartIndex += patternLengths[i];
        }
    }

private:
    consteval void appendPattern(auto pattern) noexcept
    {
        std::ranges::copy_n(pattern.storage.pattern.begin(), pattern.storage.size, buffer.end() - pattern.storage.size);
        patternLengths.back() = static_cast<std::uint8_t>(pattern.storage.size);
        patternOffsets.back() = pattern.offset;
        operations.back() = pattern.operation;
        offsetsToNextInstruction.back() = pattern.offsetToNextInstruction;
    }

    consteval void copyCurrentPool(auto& newPool) const noexcept
    {
        std::ranges::copy(buffer, newPool.buffer.begin());
        std::ranges::copy(patternLengths, newPool.patternLengths.begin());
        std::ranges::copy(patternOffsets, newPool.patternOffsets.begin());
        std::ranges::copy(operations, newPool.operations.begin());
        std::ranges::copy(offsetsToNextInstruction, newPool.offsetsToNextInstruction.begin());
    }

    template <std::size_t, std::size_t, typename>
    friend class PatternPool;

    std::array<char, BufferSize> buffer{};
    std::array<std::uint8_t, NumberOfPatterns> patternLengths{};
    std::array<std::uint8_t, NumberOfPatterns> patternOffsets{};
    std::array<CodePatternOperation, NumberOfPatterns> operations{};
    std::array<std::uint8_t, NumberOfPatterns> offsetsToNextInstruction{};
};
