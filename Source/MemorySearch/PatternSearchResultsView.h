#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <span>

struct PatternSearchResultsView {
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

    std::span<std::byte> oneByteResults;
    std::span<std::byte[4]> fourByteResults;
    std::span<std::byte[8]> eightByteResults;
};
