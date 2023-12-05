#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

#include "BytePatternConverter.h"
#include "BytePattern.h"
#include "PatternStringWildcard.h"

template <std::size_t Capacity>
struct BytePatternStorage {
    explicit(false) consteval BytePatternStorage(const char (&patternString)[Capacity])
    {
        BytePatternConverter converter{patternString};
        const auto [convertedPattern, error] = converter();
        if (error == BytePatternConverterError::NoError) {
            std::ranges::copy(convertedPattern, pattern.begin());
            size = convertedPattern.size();
        } else {
            errorOccured();
        }
    }

    template <std::size_t OtherCapacity>
    explicit consteval BytePatternStorage(BytePatternStorage<OtherCapacity> storage)
        : size{ storage.size }
    {
        std::copy_n(storage.pattern.begin(), storage.size, pattern.begin());
    }

    [[nodiscard]] explicit(false) operator BytePattern() const noexcept
    {
        return BytePattern{{pattern.data(), size}, kPatternStringWildcard};
    }

    std::array<char, Capacity> pattern{};
    std::size_t size{0};

private:
    void errorOccured();
};
