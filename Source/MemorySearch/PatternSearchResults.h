#pragma once

template <typename PatternPool>
class PatternSearchResults {
private:
    using OneBytePatternTypes = typename PatternPool::PatternTypes::template filterTransformed<UnpackStrongTypeAlias, WithSizeOf<1>::Equal>;
    using FourBytePatternTypes = typename PatternPool::PatternTypes::template filterTransformed<UnpackStrongTypeAlias, WithSizeOf<4>::Equal>;
    using EightBytePatternTypes = typename PatternPool::PatternTypes::template filterTransformed<UnpackStrongTypeAlias, WithSizeOf<8>::Equal>;

    static_assert(OneBytePatternTypes::size() + FourBytePatternTypes::size() + EightBytePatternTypes::size() == PatternPool::PatternTypes::size());
public:
    template <typename T>
    auto get() const noexcept
    {
        if constexpr (OneBytePatternTypes::template contains<T>()) {
            typename T::Type t;
            static_assert(sizeof(typename T::Type) == 1);
            static_assert(std::is_trivially_copyable_v<typename T::Type>);
            std::memcpy(reinterpret_cast<std::byte*>(&t), &oneByteResults[OneBytePatternTypes::template indexOf<T>()], 1);
            return t;
        } else if constexpr (FourBytePatternTypes::template contains<T>()) {
            typename T::Type t;
            static_assert(sizeof(typename T::Type) == 4);
            static_assert(std::is_trivially_copyable_v<typename T::Type>);
            std::memcpy(reinterpret_cast<std::byte*>(&t), &fourByteResults[FourBytePatternTypes::template indexOf<T>()], 4);
            return t;
        } else if constexpr (EightBytePatternTypes::template contains<T>()) {
            typename T::Type t;
            static_assert(sizeof(typename T::Type) == 8);
            static_assert(std::is_trivially_copyable_v<typename T::Type>);
            std::memcpy(reinterpret_cast<std::byte*>(&t), &eightByteResults[EightBytePatternTypes::template indexOf<T>()], 8);
            return t;
        } else {
            static_assert(!std::is_same_v<T, T>, "Unknown type");
        }
    }

    void store(std::size_t patternIndex, std::array<std::byte, 8> value) noexcept
    {
        if (patternIndex < oneByteResults.size()) {
            std::memcpy(&oneByteResults[patternIndex], value.data(), 1);
            return;
        }
        patternIndex -= oneByteResults.size();

        if (patternIndex < fourByteResults.size()) {
            std::memcpy(&fourByteResults[patternIndex], value.data(), 4);
            return;
        }
        patternIndex -= fourByteResults.size();

        if (patternIndex < eightByteResults.size()) {
            std::memcpy(&eightByteResults[patternIndex], value.data(), 8);
            return;
        }

        assert(false);
    }

private:
    std::array<std::byte, OneBytePatternTypes::size()> oneByteResults;
    std::array<std::byte[4], FourBytePatternTypes::size()> fourByteResults;
    std::array<std::byte[8], EightBytePatternTypes::size()> eightByteResults;
};
