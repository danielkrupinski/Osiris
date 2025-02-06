#pragma once

#include <array>
#include <bit>
#include <cstddef>

#include <Utils/Meta.h>
#include <Utils/StrongTypeAlias.h>

#include "PatternSearchResultsView.h"

template <typename PatternPool>
class PatternSearchResults {
private:
    using OneBytePatternTypes = typename PatternPool::PatternTypes::template filter<Projected<UnpackStrongTypeAlias, WithSizeOf<1>::Equal>::Value>;
    using FourBytePatternTypes = typename PatternPool::PatternTypes::template filter<Projected<UnpackStrongTypeAlias, WithSizeOf<4>::Equal>::Value>;
    using EightBytePatternTypes = typename PatternPool::PatternTypes::template filter<Projected<UnpackStrongTypeAlias, WithSizeOf<8>::Equal>::Value>;

    static_assert(OneBytePatternTypes::size() + FourBytePatternTypes::size() + EightBytePatternTypes::size() == PatternPool::PatternTypes::size());
public:
    template <typename T>
    [[nodiscard]] auto get() const noexcept
    {
        using UnpackedType = UnpackStrongTypeAliasT<T>;
        if constexpr (OneBytePatternTypes::template contains<T>())
            return std::bit_cast<UnpackedType>(oneByteResults[OneBytePatternTypes::template indexOf<T>()]);
        else if constexpr (FourBytePatternTypes::template contains<T>())
            return std::bit_cast<UnpackedType>(fourByteResults[FourBytePatternTypes::template indexOf<T>()]);
        else if constexpr (EightBytePatternTypes::template contains<T>())
            return std::bit_cast<UnpackedType>(eightByteResults[EightBytePatternTypes::template indexOf<T>()]);
        else
            static_assert(!std::is_same_v<T, T>, "Unknown type");
    }

    [[nodiscard]] PatternSearchResultsView getView() noexcept
    {
        return {oneByteResults, fourByteResults, eightByteResults};
    }

private:
    std::array<std::byte, OneBytePatternTypes::size()> oneByteResults;
    std::array<std::byte[4], FourBytePatternTypes::size()> fourByteResults;
    std::array<std::byte[8], EightBytePatternTypes::size()> eightByteResults;
};
